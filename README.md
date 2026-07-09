# flowEngine

A C++17 game engine using CMake and Conan.

## Dependencies
- spdlog/1.15.0
- glfw/3.4
- glad/2.0.8
- glm/1.0.1
- opengl/system
- entt/3.14.0
- assimp/5.4.3
- stb/cci.20230920
- physx/4.1.2

## Build

```bash
chmod +x flowLinux.sh
./flowLinux.sh Release
```

After building, run the executable from the `Targets/` directory.
```bash
./Targets/Release/bin/flow
```

## Project Structure

- `engine/` - shared library
- `editor/` - editor application
- `game/` - game executable
- `resources/` - shaders, models, textures
