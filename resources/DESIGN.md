# Flow Engine — Architecture Design Document

## 1. High-Level Module Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                         conan deps                               │
│  spdlog, glfw, glad, glm, opengl, entt, assimp, stb, physx,     │
│  sol2, lua, yaml-cpp, magic_enum, imgui, freetype                │
└─────────────────────────────────────────────────────────────────┘
                              ▲
                              │ link
              ┌───────────────┼───────────────┐
              ▼               ▼               ▼
        ┌──────────┐   ┌───────────┐   ┌──────────┐
        │ engine/  │   │  editor/  │   │  game/   │
        │  lib     │   │  exe      │   │  exe     │
        └──────────┘   └───────────┘   └──────────┘
              ▲               ▲               ▲
              │               │               │
              └───────────────┼───────────────┘
                              ▼
                     ┌──────────────┐
                     │ resources/   │
                     │ scene.yaml   │
                     │ assets.yaml  │
                     │ fonts/       │
                     │ models/      │
                     │ textures/    │
                     │ shaders/     │
                     │ scripts/     │
                     └──────────────┘
```

- **engine/** — CMake SHARED library; all subsystems.
- **editor/** — executable linking `engine` + `imgui::imgui`; implements `appInterface` via `GuiContext`.
- **game/** — minimal executable linking `engine`; constructs `Application` and runs `runContext()`.
- **resources/** — runtime data copied to binary dir via POST_BUILD.

---

## 2. Application Layer

**Files**: `engine/include/application/{context.h, interface.h, application.h}`

### `appContext` — Central Ownership
```cpp
struct appContext {
    eventDispatcher dispatcher;                    // central event bus
    std::vector<appInterface*> Layers;             // attached layers
    std::unique_ptr<appWindow> Window;             // GLFW window (1280×720)
    std::unique_ptr<ScriptContext> Scripts;
    std::unique_ptr<graphicsRenderer> Renderer;
    std::unique_ptr<physicsContext> Physics;
    std::unique_ptr<AssetRegistry> Assets;
    std::unique_ptr<DataSerializer> Serializer;
    entityRegistry Scene;                          // EnTT registry
    double deltaTime;
};
```

### `appInterface` — Layer Base
Virtual hooks (override in layers):
- `virtual void onStart() {}`
- `virtual void onUpdate() {}`

Template helpers (delegate to `m_Context`):
- Layer mgmt: `getLayer<Layer>()`, `attachLayer<Layer>(args...)`, `detachLayer<Layer>()`
- Events: `attachCallback<Event>(cb)`, `detachCallback<Event>()`, `postEvent<Event>(args...)`, `postTask(task)`
- ECS: `createEntt<Entt>(args...)`, `toEntt<Entt>(entity)`, `enttView<Entt, Comp>(task)`
- Window: `GetWindowHandle()`, `GetSceneFrame()`

### `Application` — Engine Core
- **Ctor**: allocates `appContext`, calls `RegisterCallbacks()`, then `StartScene()`.
- **`runContext()`** — main loop:
  ```cpp
  while (Window->pollEvents()) {
      UpdateDeltaTime();  // clamps dt ≤ 1/30s
      UpdateScene();      // script tick + PhysX step + pose sync
      RenderScene();      // shadow + forward pass
      UpdateLayers(showFrame);
  }
  ```
- **`StartScene()`** — deserialization entry:
  1. `Serializer->Deserialize(*Assets, "resources/projects/assets.yaml")`
  2. `Serializer->Deserialize(Scene, "resources/projects/scene.yaml")`
  3. For each `skyboxComponent` → `Renderer->InitSkybox()`
  4. For each `scriptComponent` → load+attach Lua script
  5. For each `rigidBodyComponent` → `Physics->AddRigidBody(entity)`
- **`UpdateScene()`**:
  - `scriptComponent::onUpdate(deltaTime)`
  - `Physics->Simulate(1, dt)`
  - Copy PhysX global pose → `transformComponent` (quat→euler degrees)
- **`RenderScene()`** — shadow pass → `newFrame()` → camera + lights (direct/spot/point) → draw models (`Animate`+`Draw`) → skybox → `endFrame()`.

---

## 3. ECS & Components

**File**: `engine/include/auxillaries/ECS.h`

- `entityID = entt::entity`, `entityRegistry = entt::registry`, `NENTT = entt::null`
- `Entity` — RAII wrapper (`entityRegistry*`, `entityID`); `Attach<T>()`, `Detach<T>()`, `Destroy<T>()`, `Has<T>()`, `Get<T>()`, `ID()`.

### Component Structs (14)
| Component | Fields | Notes |
|-----------|--------|-------|
| `transformComponent` | `transform3D Transform` | pos/rot/scale + `Matrix()` |
| `cameraComponent` | `camera3D Camera` | near/far/FOV + View/Proj/Frustum |
| `enttComponent` | `std::string Name` | **dead** (superseded by `infoComponent`) |
| `meshComponent` | `mesh3D Mesh` | **dead** (superseded by `modelComponent`) |
| `modelComponent` | `AssetID Model`, `AssetID Material` | main renderable |
| `pointLightComponent` | `pointLight Light` | radiance, intensity |
| `directLightComponent` | `directLight Light` | directional |
| `spotLightComponent` | `spotLight Light` | radiance, intensity, falloff, cutoff |
| `skyboxComponent` | `AssetID Sky` | cubemap |
| `animatorComponent` | `animator3D Animator` | skeletal |
| `rigidBodyComponent` | `RigidBody3D RigidBody` | `PxRigidActor* actor`, density, dynamic |
| `colliderComponent` | `Collider3D Collider` | PhysX material/mesh/shape, frictions, restitution, type |
| `scriptComponent` | `AssetID Script`, `LuaScript Instance` | `std::unique_ptr<Script>` |
| `infoComponent` | `AssetID Parent`, `std::string Name`, `AssetID UID` | **identity/tag** (UID = RandomU64) |

`infoComponent` is the de-facto identity component; no separate tag components exist.

---

## 4. Asset System

**Files**: `engine/include/graphics/textures/texture.h`, `engine/include/graphics/models/model.h`, `engine/include/auxillaries/AssetRegistry.h`

- `AssetID` = `uint64_t` (random UID or YAML-assigned)
- `AssetRef<T>` — handle with `AssetID` + typed access
- **AssetRegistry** — maps `AssetID` → loaded resource (texture2D, model3D, shader, script)
- **Loading**: on `StartScene()`, `Serializer->Deserialize(*Assets, "assets.yaml")` populates registry; `scene.yaml` references assets by `AssetID`.
- **Runtime**: components hold `AssetID` (modelComponent, skyboxComponent, scriptComponent); systems resolve via `Assets->Get<T>(id)`.

---

## 5. Serialization

**File**: `engine/include/auxillaries/serializer.h` — `DataSerializer`

- **Format**: YAML (yaml-cpp)
- **Files**:
  - `resources/projects/scene.yaml` — entity list + components
  - `resources/projects/assets.yaml` — asset catalog (textures, models, shaders, scripts)
- **API**:
  - `Serialize(entityRegistry&, path)` — writes scene
  - `Deserialize(entityRegistry&, path)` — loads scene, reconstructs entities + components
  - `Deserialize(AssetRegistry&, path)` — loads asset catalog
- **References**: `AssetID` (UID) used for cross-references (parent, material, script, skybox). `magic_enum` serializes enums by name.

---

## 6. Physics

**File**: `engine/include/physics/context.h` — `physicsContext`

Wraps PhysX 4.x (`PxPhysics`, `PxScene`, `PxControllerManager`).

| Method | Purpose |
|--------|---------|
| `CreateRigidBody(entity, ...)` | from `rigidBodyComponent` + `transformComponent` + `colliderComponent` |
| `AddRigidBody(entity)` | called in `StartScene()` |
| `RemoveRigidBody(entity)` | on entity destroy / scene unload |
| `Clear()` | destroy all actors, reset scene |
| `Simulate(substeps, dt)` | fixed-step simulation |
| `setEventCallback(cb)` | collision → script `onCollision` |

**Sync loop** (`UpdateScene`): after `Simulate`, each `rigidBodyComponent` reads `actor->getGlobalPose()`, writes back to `transformComponent` (position + quat→euler degrees).

---

## 7. Scripting

**Files**: `engine/include/scripts/ScriptContext.h`, `engine/include/scripts/Script.h`, `engine/include/scripts/LuaBindings.h`

- `ScriptContext` — owns `sol::state`, registers C++ bindings.
- `LuaScript` — per-script state (`sol::environment`), loads `.lua` from `resources/scripts/`.
- `scriptComponent` holds `AssetID Script` + `std::unique_ptr<LuaScript> Instance`.
- **Lifecycle**:
  - `StartScene()` → `Instance->Load(assetPath)` → creates env, `dofile`
  - Each frame: `Instance->Update(deltaTime)` → calls `onUpdate(dt)` in script
  - `RegisterCallbacks()` attaches C++ events (`windowResizeEvent`, `mouseDownEvent`, `keyPressEvent`, `collisionEvent`) that forward to script `onEvent(name, args)`
- **Exposed API** (in `LuaBindings.h`): entity query, transform get/set, input state, asset access, physics queries.

---

## 8. Rendering

**File**: `engine/include/graphics/renderer.h` — `graphicsRenderer`

- **OpenGL 4.5 core**, GLAD loader.
- **Passes**:
  1. **Shadow** — depth from directional light (`beginShadowPass` → `drawDepth` → `endShadowPass`)
  2. **Forward** — `newFrame()` → camera setup → light config (direct: 1, spot: N, point: N) → draw models (`Animate` + `Draw`) → skybox → `endFrame()`
- **Shaders**: 9 pairs in `resources/shaders/` (PBR, depth, skybox, etc.); hot-reload via file watch.
- **Models**: assimp loading → GPU buffers (VAO/VBO/EBO); `model3D` holds meshes + materials.
- **Skybox**: equirectangular → cubemap (`Renderer->InitSkybox()`), drawn last with depth test LEQUAL.

---

## 9. Editor

**Files**: `editor/src/editor.cpp`, `editor/src/context/context.h`, `editor/src/widget.h`, `editor/src/windows/*.h`

### `GuiContext : appInterface`
- Implements `onStart()` (imgui init, font load) + `onUpdate()` (frame, dockspace, render).
- **Fonts**: Roboto-Medium + Font Awesome (`fa-solid-900.ttf`) via merge; FreeType loader required.
- **Atlas fix** (`onUpdate`): forces CPU→GPU re-upload every frame to work around imgui 1.92 black-atlas bug (see `context.h:65-158`).

### `IWidget` Windows (5)
| Window | File | Purpose |
|--------|------|---------|
| Menubar | `windows/menubar.h` | File (Save/Open/New/Exit), Edit (Delete), Theme, Help |
| Hierarchy | `windows/hierarchy.h` | Entity tree, selection, add entity |
| Inspector | `windows/inspector.h` | Per-component editors (controls in `src/controls/`) |
| Viewport | `windows/viewport.h` | 3D scene render target, camera control |
| ResourceBrowser | `windows/resource.h` | Asset library view |

### Event Bus
- Central `eventDispatcher` in `appContext`.
- `attachCallback<Event>(cb)` / `postEvent<Event>(args...)`.
- **Events** (`engine/include/common/events.h`):
  - `SelectEvent(entityID)` — selection change
  - `windowResizeEvent(w, h)`
  - `mouseDownEvent(btn, x, y)`
  - `keyPressEvent(key, action)`
  - `collisionEvent(entityA, entityB, contact)` → forwarded to scripts

---

## 10. Events

**File**: `engine/include/common/events.h`

```cpp
struct windowResizeEvent { int width, height; };
struct mouseDownEvent { int button; double x, y; };
struct keyPressEvent { int key, scancode, action, mods; };
struct SelectEvent { entityID EnttID; };
struct collisionEvent { entityID a, b; PxContactPairHeader hdr; };
// + custom events via macro/event template
```

- `eventDispatcher` stores `std::function<void(Event&)>` per event type (type-erased via `std::any` or similar).
- Layers attach via `appInterface::attachCallback<Event>(lambda)`.

---

## 11. Build System

- **Conan** (`conanfile.txt`): 16 requires + `imgui/*:with_freetype=True` for Font Awesome.
- **CMake** (3.16+):
  - Root: `project(flowEngine)`, `option(ENABLE_ASAN ON)` → `-fsanitize=address,undefined` in Debug.
  - Engine: `add_library(engine SHARED ...)`, links 11 deps + physx; POST_BUILD `copy_directory(resources)` to `$<TARGET_FILE_DIR>/resources`.
  - Editor: `add_executable(flow)`, links `engine` + `imgui::imgui`; compiles imgui GLFW/OpenGL3 backends from conan package.
  - Game: `add_executable(game)`, links `engine` + `physx::physx`.
- **Sanitizer**: Debug builds get ASan+UBSan; define `FLOW_ENABLE_LOG` for spdlog.
- **Run**: executable must run from `Targets/{Debug,Release}/bin/` so `resources/` resolves (copied by POST_BUILD).

---

## 12. Known Issues & Workarounds

1. **imgui 1.92 dynamic atlas black texture** — fixed in `editor/src/context/context.h:onUpdate()`: every frame re-uploads `TexList` entries with white pixel at `TexUvWhitePixel` via `glTexImage2D`.
2. **Font Awesome requires FreeType** — stb_truetype fails to parse `fa-solid-900.ttf`; `imgui/*:with_freetype=True` enables FreeType rasterizer (default loader auto-selected at `imgui_draw.cpp:4308`).
3. **Dead code** — `enttComponent`, `meshComponent`, `Application::CreateEntities()` unused; kept for reference.
4. **Physics sync** — euler degrees from quat can gimbal; consider keeping quat in transform.