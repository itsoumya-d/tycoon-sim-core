#pragma once

#include <string>
#include <vector>

namespace tycoon {

struct EngineSpec {
    int cylinders = 4;
    double displacement_l = 2.0;
    bool turbo = false;
    int rpm_limit = 6500;
    double quality = 0.5;
};

struct CarSpec {
    std::string name;
    EngineSpec engine;
    double weight_kg = 1400.0;
    double drag_coeff = 0.32;
    double price = 35000.0;
    double quality = 0.5;
};

struct MarketSegment {
    std::string name = "compact";
    double monthly_demand = 800.0;
    double average_price = 32000.0;
    double price_elasticity = 1.4;
};

struct Company {
    double cash = 500000.0;
    int inventory = 0;
};

struct MonthResult {
    int produced = 0;
    int sold = 0;
    double revenue = 0.0;
    double costs = 0.0;
    double profit = 0.0;
    double cash = 0.0;
};

double engine_power_kw(const EngineSpec& spec);
double engine_torque_nm(const EngineSpec& spec);
double engine_cost(const EngineSpec& spec);
double engine_reliability(const EngineSpec& spec);
double zero_to_100_seconds(const CarSpec& car);
double top_speed_kph(const CarSpec& car);
double production_cost(const CarSpec& car);
double appeal(const CarSpec& car, const MarketSegment& segment);
int monthly_demand(const CarSpec& car, const MarketSegment& segment, int month);
MonthResult simulate_month(Company& company, const CarSpec& car,
                           const MarketSegment& segment, int units_produced, int month);

}  // namespace tycoon
