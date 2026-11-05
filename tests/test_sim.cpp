#include "tycoon/sim.hpp"

#include <cmath>
#include <iostream>
#include <string>

namespace {
int failures = 0;
void check(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << "\n";
        ++failures;
    }
}
bool close(double a, double b, double tol = 1e-9) { return std::fabs(a - b) <= tol; }
}  // namespace

int main() {
    tycoon::EngineSpec na{4, 2.0, false, 6500, 0.5};
    tycoon::EngineSpec turbo{4, 2.0, true, 6500, 0.5};
    check(tycoon::engine_power_kw(turbo) > tycoon::engine_power_kw(na), "turbo adds power");
    check(tycoon::engine_reliability({4, 2.0, false, 8000, 0.5}) < tycoon::engine_reliability(na), "rpm lowers reliability");
    check(tycoon::engine_cost(turbo) > tycoon::engine_cost(na), "turbo adds cost");

    tycoon::CarSpec light{"light", turbo, 1100.0, 0.30, 35000.0, 0.6};
    tycoon::CarSpec heavy{"heavy", turbo, 1800.0, 0.30, 35000.0, 0.6};
    check(tycoon::zero_to_100_seconds(light) < tycoon::zero_to_100_seconds(heavy), "lighter is faster");
    check(tycoon::top_speed_kph(light) > 80.0, "top speed positive");

    tycoon::MarketSegment segment{"compact", 1000.0, 30000.0, 1.5};
    tycoon::CarSpec cheap{"cheap", na, 1300.0, 0.32, 25000.0, 0.5};
    tycoon::CarSpec expensive{"expensive", na, 1300.0, 0.32, 45000.0, 0.5};
    check(tycoon::monthly_demand(cheap, segment, 3) > tycoon::monthly_demand(expensive, segment, 3), "price sensitivity");

    tycoon::Company a{500000.0, 0};
    tycoon::Company b{500000.0, 0};
    auto one = tycoon::simulate_month(a, cheap, segment, 500, 3);
    auto two = tycoon::simulate_month(b, cheap, segment, 500, 3);
    check(close(one.cash, two.cash), "deterministic cash");
    check(one.sold >= 0 && one.produced == 500, "month output");
    check(tycoon::production_cost(cheap) > 0.0, "production cost positive");

    if (failures == 0) {
        std::cout << "all tests passed\n";
        return 0;
    }
    return 1;
}
