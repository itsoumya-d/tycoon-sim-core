#include "tycoon/sim.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <sstream>
#include <stdexcept>

namespace tycoon {

namespace {
double clamp(double value, double low, double high) {
    return std::max(low, std::min(high, value));
}
}  // namespace

double engine_power_kw(const EngineSpec& spec) {
    const double base = spec.displacement_l * 42.0;
    const double rpm = static_cast<double>(spec.rpm_limit) / 6000.0;
    const double boost = spec.turbo ? 1.38 : 1.0;
    const double quality = 0.85 + spec.quality * 0.30;
    return std::max(0.0, base * rpm * boost * quality);
}

double engine_torque_nm(const EngineSpec& spec) {
    const double peak_rpm = std::max(2500.0, 0.70 * spec.rpm_limit);
    return engine_power_kw(spec) * 9549.0 / peak_rpm;
}

double engine_cost(const EngineSpec& spec) {
    return 900.0 + spec.displacement_l * 1100.0 + (spec.turbo ? 1600.0 : 0.0) + spec.quality * 2500.0;
}

double engine_reliability(const EngineSpec& spec) {
    const double rpm_penalty = std::max(0.0, spec.rpm_limit - 6500.0) / 25000.0;
    const double turbo_penalty = spec.turbo ? 0.025 : 0.0;
    return clamp(0.97 - rpm_penalty - turbo_penalty + spec.quality * 0.06, 0.50, 0.995);
}

double zero_to_100_seconds(const CarSpec& car) {
    const double power = std::max(1.0, engine_power_kw(car.engine));
    const double seconds = 0.70 * car.weight_kg / power / (0.85 + car.quality * 0.15);
    return clamp(seconds, 2.5, 25.0);
}

double top_speed_kph(const CarSpec& car) {
    const double power = std::max(1.0, engine_power_kw(car.engine));
    const double speed = 35.0 * std::cbrt(power) / (0.55 + car.drag_coeff);
    return clamp(speed, 80.0, 420.0);
}

double production_cost(const CarSpec& car) {
    return 6000.0 + car.weight_kg * 4.0 + engine_cost(car.engine) + car.quality * 5000.0;
}

double appeal(const CarSpec& car, const MarketSegment& segment) {
    const double power = std::max(1.0, engine_power_kw(car.engine));
    const double power_to_weight = power / car.weight_kg * 1000.0;
    const double reliability = engine_reliability(car.engine);
    const double price_value = segment.average_price / std::max(1.0, car.price);
    const double score = 0.35 * clamp(power_to_weight / 120.0, 0.0, 1.5)
                       + 0.25 * reliability
                       + 0.20 * car.quality
                       + 0.20 * clamp(price_value, 0.0, 1.5);
    return clamp(score, 0.0, 1.5);
}

int monthly_demand(const CarSpec& car, const MarketSegment& segment, int month) {
    const double price_ratio = segment.average_price / std::max(1.0, car.price);
    const double elasticity = std::pow(clamp(price_ratio, 0.25, 4.0), segment.price_elasticity);
    const double seasonality = 1.0 + 0.08 * std::sin(static_cast<double>(month) * 0.52);
    const double raw = segment.monthly_demand * appeal(car, segment) * elasticity * seasonality;
    return std::max(0, static_cast<int>(std::lround(raw)));
}

std::uint64_t state_hash(const Company& company) {
    std::uint64_t hash = 1469598103934665603ULL;
    auto mix = [&hash](std::uint64_t value) {
        hash ^= value;
        hash *= 1099511628211ULL;
    };
    const auto cash_bits = static_cast<std::uint64_t>(static_cast<std::int64_t>(std::llround(company.cash * 100.0)));
    mix(cash_bits);
    mix(static_cast<std::uint64_t>(company.inventory));
    return hash;
}

std::string serialize_company(const Company& company) {
    std::ostringstream out;
    out.precision(17);
    out << "company-v1 cash=" << company.cash << " inventory=" << company.inventory;
    return out.str();
}

Company deserialize_company(const std::string& payload) {
    std::istringstream in(payload);
    std::string version, cash_token, inventory_token;
    if (!(in >> version >> cash_token >> inventory_token) || version != "company-v1") {
        throw std::invalid_argument("invalid company save payload");
    }
    const auto cash_pos = cash_token.find('=');
    const auto inventory_pos = inventory_token.find('=');
    if (cash_pos == std::string::npos || inventory_pos == std::string::npos) {
        throw std::invalid_argument("invalid company save fields");
    }
    Company company;
    company.cash = std::stod(cash_token.substr(cash_pos + 1));
    company.inventory = std::stoi(inventory_token.substr(inventory_pos + 1));
    if (company.inventory < 0 || !std::isfinite(company.cash)) {
        throw std::invalid_argument("invalid company state");
    }
    return company;
}

std::uint64_t replay_hash(const CarSpec& car, const MarketSegment& segment,
                          int months, int units_per_month, std::uint64_t seed) {
    if (months < 0 || units_per_month < 0) throw std::invalid_argument("months and units_per_month must be non-negative");
    Company company;
    company.cash = 500000.0 + static_cast<double>(seed % 1000);
    for (int month = 0; month < months; ++month) {
        simulate_month(company, car, segment, units_per_month, month);
    }
    return state_hash(company);
}

MonthResult simulate_month(Company& company, const CarSpec& car,
                           const MarketSegment& segment, int units_produced, int month) {
    if (units_produced < 0) throw std::invalid_argument("units_produced must be non-negative");
    const int demand = monthly_demand(car, segment, month);
    const int available = company.inventory + units_produced;
    const int sold = std::min(available, demand);
    const double revenue = sold * car.price;
    const double costs = units_produced * production_cost(car) + 50000.0;
    const double profit = revenue - costs;
    company.cash += profit;
    company.inventory = available - sold;
    return MonthResult{units_produced, sold, revenue, costs, profit, company.cash};
}

}  // namespace tycoon
