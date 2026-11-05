# tycoon-sim-core

[![ci](https://github.com/itsoumya-d/tycoon-sim-core/actions/workflows/ci.yml/badge.svg)](https://github.com/itsoumya-d/tycoon-sim-core/actions/workflows/ci.yml)

A deterministic C++17 gameplay simulation core for an automotive tycoon game:

- Engine design: displacement, cylinders, turbo, RPM limit, quality.
- Derived power, torque, reliability and engine cost.
- Car performance: 0-100 km/h, top speed, production cost.
- Market appeal, price elasticity, seasonality and monthly demand.
- Company cash, inventory, production, sales, revenue, costs and profit.
- Save/load serialization with state validation.
- Deterministic replay hashing for regression and desync checks.
- CMake build and a dependency-free test runner in CI.

This is a production-shaped simulation core, not a claim of shipped Unreal or
Unity title experience. It is the gameplay/economy layer a team could embed
behind a UI; rendering, assets, networking and engine integration are out of
scope.

## Build and test

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/tycoon_demo
```

## License

MIT
