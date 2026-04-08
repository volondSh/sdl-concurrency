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
- ECS-based rendering with EnTT
- Concurrency experiments and multithreading patterns
- Configurable application settings (JSON config)
- Logging with spdlog

## Demo: Starfield

The current demo renders 2500 randomly placed monochrome stars as points on a black background at 60 FPS in a 4000×4000 world:

- Star positions are generated procedurally using `std::mt19937`
- Each star has `Position` and `Color` ECS components
- 500 stars have a `Velocity` component and drift at 5–30 pixels/second in random directions
- Stars wrap around world edges — they reappear on the opposite side when leaving the world boundary

### Controls

| Key | Action |
|---|---|
| `W` / `S` | Move camera up / down |
| `A` / `D` | Move camera left / right |
| `Q` / `E` | Zoom out / in (0.1x–5x, multiplicative) |
| `F11` | Toggle fullscreen (stars regenerate for new resolution) |
| `Escape` | Exit |

### Architecture

The render pipeline consists of three ECS systems executed each frame:

1. **MovementSystem** — updates `Position` based on `Velocity × dt` with world wrapping
2. **TransformSystem** — converts `Position + Camera → ScreenPosition` using camera offset and zoom
3. **RenderSystem** — draws visible stars using `ScreenPosition`, skipping those outside the screen

- Press `Escape` or close the window to exit

## Dependencies

Managed automatically via [CPM.cmake](cmake/CPM.cmake):

| Library | Version | Purpose |
|---|---|---|
| [SDL3](https://github.com/libsdl-org/SDL) | 3.4.4 | Graphics, events, windowing |
| [spdlog](https://github.com/gabime/spdlog) | 1.14.1 | Logging |
| [EnTT](https://github.com/skypgart/entt) | 3.16.0 | Entity-Component-System |
| [nlohmann/json](https://github.com/nlohmann/json) | 3.12.0 | JSON configuration |
| [GoogleTest](https://github.com/google/googletest) | 1.17.0 | Unit testing |

### Build-time options

| Option | Default | Description |
|---|---|---|
| `BUILD_TESTS` | `ON` | Build GoogleTest unit tests |
| `ENABLE_CCACHE` | `ON` | Enable ccache compiler launcher |

## Build requirements

- **CMake** ≥ 3.25
- **C++23** compiler (GCC 13+, clang 17+)
- **Ninja** build system
- **ccache** (optional, speeds up rebuilds)

## License

This project is licensed under the MIT License - see the [LICENSE file](LICENSE) for details.
