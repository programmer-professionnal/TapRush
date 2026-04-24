# TapRush - Clicker Game

A customizable clicker game built with C++ and raylib.

## How to Build

```powershell
# Configure and build
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

The executable will be in `build/TapRush.exe`.

## How to Play

- Click the big button to earn points
- Buy upgrades from the right panel to earn automatic clicks per second
- Upgrades scale in cost

## Customization

Modify these files to customize:

- **`src/Game.cpp`** - Game logic, upgrades, UI
- **`src/Button.cpp`** - Button rendering
- **`include/Game.h`** - Game class definition

### Adding New Upgrades

In `Game.cpp`, find `LoadUpgrades()` and add new entries:

```cpp
{"Upgrade Name", cost, clicksPerSecond, count},
```

### Adding New Features

1. Add methods to `Game.h`
2. Implement in `Game.cpp`
3. Draw in the `Draw()` function

## Required Dependencies

- CMake 3.10+
- MinGW (GCC for Windows)
- raylib 5.0

## License

Apache 2.0
