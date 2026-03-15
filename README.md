# 2048 Game - Enhanced Innovation Version

This is the 2026 C++ Course Project for Nankai University.

## Project Overview

This project is based on the traditional 2048 game with improvements and innovations to avoid monotony and provide a fresh gaming experience.

Currently, only the traditional 2048 version has been implemented.

## Technical Stack

- Framework: Qt 6
- Build System: CMake
- Language: C++

## Features

### Implemented Features

- ✅ Classic 2048 gameplay mechanics
- ✅ Smooth tile movements and animations
- ✅ Score tracking system

### Planned Features

- 🚀 New game modes
- 🚀 Special power-ups and abilities
- 🚀 Themed visual effects
- 🚀 Achievement system
- 🚀 Multiplayer support
- 🚀 Custom board sizes

## Building the Project

### Prerequisites

- Qt 6.0 or higher
- CMake 3.16 or higher
- C++17 compatible compiler

### Build Instructions

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Running the Game

After building, run the executable:

```bash
./advanced-2048  # On Linux/Mac
# or
advanced-2048.exe  # On Windows
```

## Project Structure

```cpp
advanced-2048/
├── src/           # Source files
├── include/       # Header files
├── resources/     # Game resources (images, sounds)
├── tests/         # Unit tests
└── CMakeLists.txt # CMake build configuration
```

## Development Status

- Current Version: 0.1.0 (Classic 2048 implementation)
- Next Milestone: Enhanced features and game modes

## Contributing

Contributions are welcome! Feel free to submit pull requests or open issues for bugs and feature suggestions.

## License

This application uses Qt framework licensed under the GNU Lesser General Public License v3.0.

## Acknowledgments

- Original 2048 game by Gabriele Cirulli
- Qt framework community