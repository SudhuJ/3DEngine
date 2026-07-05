# flowEngine

A C++17 game engine using CMake and Conan.

## Dependencies

- spdlog, glfw, glad, glm, EnTT, assimp, stb

## Build

```bash
chmod +x flowLinux.sh
./flowLinux.sh Debug
./flowLinux.sh Release
```

## Project Structure

- `engine/` - shared library
- `editor/` - editor application
- `game/` - game executable
- `resources/` - shaders, models, textures
