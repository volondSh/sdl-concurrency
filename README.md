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
- Configurable application and scene parameters (JSON config)
- ImGui settings menu with key rebinding
- FPS overlay with SDL3_ttf and JetBrains Mono font
- Per-frame profiler with timing breakdown
- Logging with spdlog

## Demo: Starfield

The current demo renders stars as monochrome points on a black background at 60 FPS. All parameters are configurable in `app_config.json` under the `scene` section.

- Star positions are generated procedurally using `std::mt19937` within `worldWidth × worldHeight` centered at (0, 0)
- Each star has `Position` and `Color` ECS components
- Configurable number of twinkling stars with adjustable frequency and amplitude
- Moving stars have a `Velocity` component and drift at 5–30 pixels/second in random directions
- Stars wrap around world edges — they reappear on the opposite side when leaving the world boundary

### Overlay

A debug overlay in the top-left corner (toggled with `` ` ``):

- Green monospace text via SDL3_ttf with JetBrains Mono
- **FPS** — frames per second
- **Entities** — total `Position` entities
- **Moving** — entities with `Velocity` component
- **Twinkle** — entities with `Twinkle` component
- **Visible** — entities currently on screen, broken down by `Moving` and `Twinkle`
- **Per-system timing** — movement, twinkle, transform, culling, render + total
- Updates once per second with averaged values

### Controls

| Key | Action |
|---|---|
| `W` / `S` | Move camera up / down |
| `A` / `D` | Move camera left / right |
| `Q` / `E` | Zoom out / in (0.1x–5x, multiplicative) |
| `` ` `` | Toggle FPS overlay |
| `F11` | Toggle fullscreen (stars regenerate for new resolution) |
| `Escape` | Toggle menu |

### Architecture

The render pipeline consists of four ECS systems executed each frame:

1. **MovementSystem** — updates `Position` based on `Velocity × dt` with world wrapping
2. **TwinkleSystem** — modulates star brightness via sinusoidal phase
3. **TransformSystem** — converts `Position + Camera → ScreenPosition` using camera offset and zoom
4. **CullingSystem** — logs which entities are currently visible
5. **RenderSystem** — draws only `Visible` entities (skipped off-screen stars)

### Settings Menu

Press `Escape` to open the ImGui settings menu:

- **Settings** — sliders for star count, world size, and camera parameters
  - `Save & Apply` — writes to JSON and regenerates the scene
  - `Reset to Defaults` — restores original values
- **Controls** — click a key binding then press any key to rebind

### Profiling

Each system is wrapped in a `Profiler::Scope` for per-frame timing measurement:

- Results sorted by call order (not by duration)
- `accumulate()` copies and clears per-frame accumulators
- RAII scope ensures timing is captured even on early returns

## Dependencies

Managed automatically via [CPM.cmake](cmake/CPM.cmake):

| Library | Version | Purpose |
|---|---|---|
| [SDL3](https://github.com/libsdl-org/SDL) | 3.4.4 | Graphics, events, windowing |
| [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf) | 3.2.2 | Text rendering (FPS overlay) |
| [Dear ImGui](https://github.com/ocornut/imgui) | 1.92.7 | Settings menu UI |
| [spdlog](https://github.com/gabime/spdlog) | 1.14.1 | Logging |
| [EnTT](https://github.com/skypgart/entt) | 3.16.0 | Entity-Component-System |
| [nlohmann/json](https://github.com/nlohmann/json) | 3.12.0 | JSON configuration |
| [GoogleTest](https://github.com/google/googletest) | 1.17.0 | Unit testing |

### Fonts

The FPS overlay uses **JetBrains Mono** (OFL 1.1 license), bundled in `resources/fonts/`. No manual setup required.

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
