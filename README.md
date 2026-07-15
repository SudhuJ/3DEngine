# flowEngine

A C++17 game engine built with CMake and Conan.

Shared `engine` library + thin executables: `editor` (ImGui dockspace UI) and `game` (headless launcher). OpenGL 4.5, EnTT ECS, PhysX 4.x, Lua (sol2), YAML serialization.

## Dependencies

All managed via Conan (`conanfile.txt`):

- spdlog/1.15.0
- glfw/3.4
- glad/2.0.8
- glm/1.0.1
- opengl/system
- entt/3.14.0
- assimp/5.4.3
- stb/cci.20230920
- physx/4.1.2
- sol2/3.5.0
- lua/5.4.6
- yaml-cpp/0.9.0
- magic_enum/0.9.7
- imgui/1.92.8-docking
- freetype/2.14.3

## Quick Start

```bash
# 1. Install Conan (if missing)
pipx install conan   # or: brew install conan / pip install conan

# 2. Configure & install deps (first run, or after conanfile changes)
conan install . --output-folder=build --build=missing -s build_type=Debug

# 3. Configure CMake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# 4. Build
cmake --build build --config Debug

# 5. Run editor (must run from binary dir so resources/ resolves)
./build/Targets/Debug/bin/flow
```

### Release build

```bash
conan install . --output-folder=build --build=missing -s build_type=Release
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/Targets/Release/bin/flow
```

## Project Layout

```
engine/       # shared library (ECS, physics, rendering, scripting, assets, serialization)
editor/       # ImGui dockspace editor application
game/         # minimal game executable (links engine)
resources/    # shaders, models, textures, fonts, scene.yaml, assets.yaml
build/        # CMake + Conan output (gitignored)
```

## Architecture Choices

- **Shared engine library** — `engine` is a CMake SHARED lib; `editor` and `game` link it.
- **Layered application core** — `appInterface` defines ` owns `appContext` (serializer, assets, scene, physics); layers attach via `onStart`/`onUpdate` (see `engine/include/application/interface.h`).
- **EnTT ECS + YAML** — entities + 14 component types serialized to/from `resources/projects/scene.yaml` and `resources/assets.yaml` via `DataSerializer`.
- **PhysX 4.x** — rigid bodies created from `rigidBodyComponent`; sync loop in `UpdateScene()`.
- **Lua scripting (sol2)** — `scriptComponent` references `.lua` assets; C++↔Lua callbacks registered in `RegisterCallbacks()`.
- **ImGui editor** — dockspace layout with 5 widget windows (Menubar, Hierarchy, Inspector, Viewport, ResourceBrowser); event bus for selection/resize/input.
- **FreeType font loader required** — Font Awesome icons (`fa-solid-900.ttf`) need imgui's FreeType rasterizer; enabled via `imgui/*:with_freetype=True` in conanfile.

## Known Fix

imgui 1.92 dynamic atlas can hand back a black texture. Workaround: forced re-upload every frame in `editor/src/context/context.h:onUpdate()` — binds CPU pixels, ensures white pixel at `TexUvWhitePixel`, re-`glTexImage2D` into each `TexList` entry.