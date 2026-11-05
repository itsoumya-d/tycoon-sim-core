# tycoon-sim-core

[![ci](https://github.com/itsoumya-d/tycoon-sim-core/actions/workflows/ci.yml/badge.svg)](https://github.com/itsoumya-d/tycoon-sim-core/actions/workflows/ci.yml)

A deterministic C++17 simulation core for an automotive tycoon game:

- Engine design: displacement, cylinders, turbo, RPM limit, quality.
- Derived power, torque, reliability and engine cost.
- Car performance: 0-100 km/h, top speed, production cost.
- Market appeal, price elasticity, seasonality and monthly demand.
- Company cash, inventory, production, sales, revenue, costs and profit.
- CMake build and a dependency-free test runner in CI.

This is a **reference implementation**, not a claim of production Unreal Engine
or commercial game-development experience. It is the simulation layer a game
team could embed behind a UI; it does not include rendering, assets or Unreal
integration.

## Build and test

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/tycoon_demo
```

## License

MIT
