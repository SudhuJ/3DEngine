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
// #include "interface.h"

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

            deltaTime = 0.0;
        }

        FLOW_INLINE ~appContext() {
            // for (auto& layer : Layers) {
            //     FLOW_DELETE(layer);
            // }
            Layers.clear();
        }

        FLOW_INLINE void StartRuntime() {
            // Scene.view<skyboxComponent>().each([this](auto id, auto& comp) {
            //     auto& skybox = Assets->Get<SkyboxAsset>(comp.Sky);
            //     auto skyTex = std::make_shared<texture2D>(skybox.EnvMap);
            //     Renderer->InitSkybox(skybox.Data, skyTex, skybox.Size);
            // });
            Scene.view<rigidBodyComponent>().each([this](auto id, auto& comp) {
                Entity ent(&Scene, id);
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
                auto name = Scripts->loadScript(script.Source);
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
        entityRegistry Scene;
        double deltaTime = 0.0;
        engineMode Mode = engineMode::EDITOR;

        std::string sceneSnapshot;
    };
}
