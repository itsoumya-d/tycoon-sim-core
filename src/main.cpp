#include "tycoon/sim.hpp"

#include <iostream>

int main() {
    tycoon::EngineSpec engine{4, 2.0, true, 7000, 0.7};
    tycoon::CarSpec car{"Aurora GT", engine, 1380.0, 0.30, 42000.0, 0.7};
    tycoon::MarketSegment market{"sport compact", 900.0, 38000.0, 1.3};
    tycoon::Company company{750000.0, 0};

    std::cout << "Power: " << tycoon::engine_power_kw(engine) << " kW\n";
    std::cout << "Torque: " << tycoon::engine_torque_nm(engine) << " Nm\n";
    std::cout << "Reliability: " << tycoon::engine_reliability(engine) << "\n";
    std::cout << "0-100: " << tycoon::zero_to_100_seconds(car) << " s\n";
    std::cout << "Top speed: " << tycoon::top_speed_kph(car) << " km/h\n";
    std::cout << "Production cost: " << tycoon::production_cost(car) << "\n";

    for (int month = 1; month <= 12; ++month) {
        auto result = tycoon::simulate_month(company, car, market, 600, month);
        std::cout << "Month " << month << ": sold " << result.sold
                  << ", profit " << result.profit << ", cash " << result.cash << "\n";
    }
    return 0;
}
