# sdl-concurrency

Modern C++ wrappers around the SDL3 C API with experiments in concurrency and multithreading.

## Quick Start

### Windows (Ninja + GCC)

Open a shell where `gcc`, `g++`, and `ninja` are available (e.g., MinGW/MSYS2) and run:

```powershell
cmake --preset windows-gcc-debug
cmake --build --preset build-windows-gcc-debug
ctest --preset test-windows-gcc-debug
```

```powershell
cmake --preset windows-gcc-release
cmake --build --preset build-windows-gcc-release
ctest --preset test-windows-gcc-release
```

### Linux (Ninja + clang)

```bash
cmake --preset linux-clang-debug
cmake --build --preset build-linux-clang-debug
ctest --preset test-linux-clang-debug
```

```bash
cmake --preset linux-clang-release
cmake --build --preset build-linux-clang-release
ctest --preset test-linux-clang-release
```

## Features
- Modern C++23 codebase
- Object-oriented wrappers for SDL3
- Concurrency experiments and multithreading patterns
- Configurable application settings
- Logging with spdlog

## Dependencies
- SDL3
- spdlog
- nlohmann/json
- GoogleTest (for tests)

## License
This project is licensed under the MIT License - see the [LICENSE file](LICENSE) for details.
