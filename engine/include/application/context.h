#pragma once
#include "common/core.h"
#include "common/event.h"

#include "window/window.h"
#include "graphics/renderer.h"
#include "physics/context.h"
#include "scripts/context.h"

#include "auxillaries/ECS.h"
#include "auxillaries/assets.h"
#include "auxillaries/serializer.h"
#include "auxillaries/project.h"


namespace flow {
    struct appInterface;

    enum class engineMode : uint8_t {
            EDITOR,
            PLAY_IN_EDITOR,
            STANDALONE_GAME
        };

    struct appContext {

        FLOW_INLINE appContext() {
            Window = std::make_unique<appWindow>(&dispatcher, 1280, 720, "Flow Engine");
            Physics = std::make_unique<physicsContext>();
            Scripts = std::make_unique<ScriptContext>(&Scene, Window.get());
            Renderer = std::make_unique<graphicsRenderer>(1280, 720);
            Assets = std::make_unique<AssetRegistry>();
            Serializer = std::make_unique<DataSerializer>();
            Project = std::make_unique<ProjectManager>(Assets.get(), &Scene, Serializer.get());

            deltaTime = 0.0;
        }

        FLOW_INLINE ~appContext() {
            // for (auto& layer : Layers) {
            //     FLOW_DELETE(layer);
            // }
            Layers.clear();
        }

        FLOW_INLINE void EnsureMeshCollider(Entity& entity) {
            if (!entity.template Has<colliderComponent>()) return;
            auto& collider = entity.template Get<colliderComponent>().Collider;
            if (collider.Type != MESH || collider.TriangleMesh.triangleMesh) return;

            if (entity.template Has<modelComponent>()) {
                auto& model = entity.template Get<modelComponent>();
                auto& asset = Assets->Get<ModelAsset>(model.Model);        // AssetRegistry access
                auto* sm = dynamic_cast<staticModel*>(asset.Data.get());
                if (sm && !sm->GetMeshes().empty()) {
                    collider.TriangleMesh = Physics->CookTriangleMesh(sm->GetMeshes()[0].Data);
                }
            }
        }

        // (re)builds cubemap + IBL maps for every skybox in the scene
        FLOW_INLINE void InitSkyboxes() {
            Scene.view<skyboxComponent>().each([this](auto id, auto& comp) {
                auto& skybox = Assets->Get<SkyboxAsset>(comp.Sky);
                if (!skybox.EnvMap) {
                    FLOW_WARN("Skybox '{}' has no environment map.", skybox.Name);
                    return;
                }
                Renderer->InitSkybox(skybox.Data, skybox.EnvMap, skybox.Size);
            });
        }

        // stops play mode, swaps in the project at dir and rebuilds GPU-side scene state
        FLOW_INLINE flow::Project* OpenProject(const std::string& dir) {
            if (Mode == engineMode::PLAY_IN_EDITOR) {
                StopRuntime();
            }
            auto* project = Project->OpenProject(dir);
            InitSkyboxes();
            return project;
        }

        FLOW_INLINE void SpawnDefaultScene() {
            // Skybox
            auto skyAsset = Assets->AddSkybox(RandomU64(), "resources/textures/sky.hdr", 2048);
            Entity skyEnt(&Scene);
            skyEnt.Attach<infoComponent>().Name = "Sky";
            skyEnt.Attach<skyboxComponent>().Sky = skyAsset->UID;
            skyEnt.Attach<transformComponent>();

            // Ground plane
            auto planeModel = Assets->AddModel(RandomU64(), "resources/models/cube.fbx");
            auto planeMat   = Assets->AddMaterial(RandomU64(), "Ground");
            planeMat->Data.Albedo = glm::vec3(0.25f, 0.0f, 1.0f);

            Entity plane(&Scene);
            plane.Attach<infoComponent>().Name = "Ground";
            plane.Attach<rigidBodyComponent>();
            auto& pCollider = plane.Attach<colliderComponent>().Collider;
            pCollider.Type = BOX;
            pCollider.StaticFriction = 0.5f;
            pCollider.DynamicFriction = 0.5f;
            auto& pModel = plane.Attach<modelComponent>();
            pModel.Model    = planeModel->UID;
            pModel.Material = planeMat->UID;
            auto& pT = plane.Attach<transformComponent>().Transform;
            pT.Translate = glm::vec3(0.0f, -1.0f, 0.0f);
            pT.Scale = glm::vec3(100.0f, 1.0f, 100.0f);

            // Direct light
            Entity sun(&Scene);
            sun.Attach<infoComponent>().Name = "Sun";
            auto& sunLight = sun.Attach<directLightComponent>().Light;
            sunLight.Intensity = 2.5f;
            sunLight.Radiance  = glm::vec3(2.5f);
            auto& sunT = sun.Attach<transformComponent>().Transform;
            sunT.Rotate = glm::vec3(-60.0f, 0.0f, 0.0f);

            // Movement + PlayerCharacter
            auto scriptAsset = Assets->AddScript(RandomU64(), "resources/scripts/testScript.lua");
            Entity player(&Scene);
            player.Attach<infoComponent>();
            player.Attach<cameraComponent>();
            player.Attach<scriptComponent>().Script = scriptAsset->UID;
            auto& playerCC = player.Attach<charControllerComponent>().Controller;
            playerCC.SpawnPosition = glm::vec3(0.0f, 0.0f, 0.0f);
            auto& playerT = player.Attach<transformComponent>().Transform;
            playerT.Translate = playerCC.SpawnPosition;
        }

        FLOW_INLINE void StartRuntime() {
            Scene.view<rigidBodyComponent>().each([this](auto id, auto& comp) {
                Entity ent(&Scene, id);
                EnsureMeshCollider(ent);
                Physics->AddRigidBody(ent);
            });
            Scene.view<charControllerComponent>().each([this](auto id, auto& comp) {
                Entity ent(&Scene, id);
                auto& transform = ent.Get<transformComponent>().Transform;
                transform.Translate = comp.Controller.SpawnPosition;
                comp.Controller.Yaw = comp.Controller.SpawnYaw;
                comp.Controller.Pitch = 0.0f;
                Physics->AddCharacterController(ent);
            });
            Scene.view<scriptComponent>().each([this](auto id, auto& comp) {
                auto& script = Assets->Get<ScriptAsset>(comp.Script);
                auto name = Scripts->loadScript(Assets->Resolve(script.Source));
                Entity ent(&Scene, id);
                Scripts->attachScript(ent, name);
            });
        }

        FLOW_INLINE void StopRuntime() {
            if (sceneSnapshot.empty()) return;
            Scene.view<charControllerComponent>().each([this](auto entity, auto& comp) {
                if (comp.ControllerPtr) {
                    comp.ControllerPtr->release();
                }
                if (comp.Material) {
                    comp.Material->release();
                    comp.Material = nullptr;
                }
            });
            Scene.view<rigidBodyComponent>().each([this](auto entity, auto& comp) {
                if (comp.RigidBody.actor) {
                    Physics->RemoveActor(comp.RigidBody.actor);
                    comp.RigidBody.actor->release();
                    comp.RigidBody.actor = nullptr;
                }
            });
            Scene.view<colliderComponent>().each([this](auto entity, auto& comp) {
                if (comp.Collider.Shape) {
                    comp.Collider.Shape->release();
                    comp.Collider.Shape = nullptr;
                }
                if (comp.Collider.Material) {
                    comp.Collider.Material->release();
                    comp.Collider.Material = nullptr;
                }
            });
            Scene.view<scriptComponent>().each([this](auto entity, auto& comp) {
                comp.Instance = LuaScript();
            });
            // Scene.clear();
            Serializer->DeserializeFromString(Scene, sceneSnapshot);
            Mode = engineMode::EDITOR;
            glfwSetInputMode(Window->getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        eventDispatcher dispatcher;
        std::vector<appInterface*> Layers;
        std::unique_ptr<appWindow> Window;
        std::unique_ptr<graphicsRenderer> Renderer;
        std::unique_ptr<physicsContext> Physics;
        std::unique_ptr<ScriptContext> Scripts;
        std::unique_ptr<AssetRegistry> Assets;
        std::unique_ptr<DataSerializer> Serializer;
        std::unique_ptr<ProjectManager> Project;

        entityRegistry Scene;
        double deltaTime = 0.0;
        engineMode Mode = engineMode::EDITOR;

        std::string sceneSnapshot;
    };
}
