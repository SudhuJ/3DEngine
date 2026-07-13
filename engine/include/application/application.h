#pragma once
#include "interface.h"

namespace flow {
    struct Application : appInterface
    {
        FLOW_INLINE Application(){
            m_LayerID = TypeID<Application>();
            m_Context = new appContext();

            RegisterCallbacks();
            StartScene();
        }

        FLOW_INLINE ~Application()
        {
            for (auto layer : m_Context->Layers){
                FLOW_DELETE(layer);
            }
            FLOW_DELETE(m_Context);
        }

        FLOW_INLINE void runContext(){
            while(m_Context->Window->pollEvents()) {
                UpdateDeltaTime();
                UpdateScene();
                RenderScene();
                UpdateLayers();
            }
        }

        private:
            FLOW_INLINE void RegisterCallbacks() {
                m_Context->Physics->setEventCallback([this](auto e) {
                    // scripting
                    if (!m_Context->Scene.valid(e.Entity1) ||
                        !m_Context->Scene.valid(e.Entity2)) {
                        return;
                    }

                    postTask([this, e] {
                        auto entity1 = toEntt<Entity>(e.Entity1);
                        auto entity2 = toEntt<Entity>(e.Entity2);

                        if (m_Context->Scene.valid(entity1) && entity1.template Has<scriptComponent>()) {
                            entity1.template Get<scriptComponent>().Instance->onCollision(e.Entity2);
                        }

                        if (m_Context->Scene.valid(entity2) && entity2.template Has<scriptComponent>()) {
                            entity2.template Get<scriptComponent>().Instance->onCollision(e.Entity1);
                        }
                    });
                });

                attachCallback<windowResizeEvent>([this] (auto e) {
                    m_Context->Renderer->Resize(e.width, e.height);
                    enttView<Entity, scriptComponent>([e] (auto entity, auto& script) {
                        if (script.Instance) {
                            script.Instance->onResize(e.width, e.height);
                        }
                    });
                });

                attachCallback<mouseDownEvent>([this] (auto e) {
                    enttView<Entity, scriptComponent>([e] (auto entity, auto& script) {
                        if (script.Instance) {
                            script.Instance->onMouseDown(e.button);
                        }
                    });
                });

                attachCallback<keyPressEvent>([this] (auto e) {
                    enttView<Entity, scriptComponent>([e] (auto entity, auto& script) {
                        if (script.Instance) {
                            script.Instance->onKeyDown(e.key);
                        }
                    });
                });
            }

            FLOW_INLINE void UpdateDeltaTime() {
                static double sLastTime = glfwGetTime();
                double currentTime = glfwGetTime();
                // clamp time to prevent physics during resize
                m_Context->deltaTime = glm::min(currentTime - sLastTime, 1.0 / 30.0);
                sLastTime = currentTime;
            }

            FLOW_INLINE void CreateEntities() {
                auto skyboxAsset = m_Context->Assets->AddSkybox(RandomU64(), "resources/textures/sky.hdr", 2048);
                auto robotAsset = m_Context->Assets->AddModel(RandomU64(), "resources/models/walking.fbx", true);
                auto scriptAsset = m_Context->Assets->AddScript(RandomU64(), "resources/scripts/testScript.lua");
                auto sphereAsset = m_Context->Assets->AddModel(RandomU64(), "resources/models/sphere.fbx");
                auto cubeAsset = m_Context->Assets->AddModel(RandomU64(), "resources/models/cube.fbx");
                auto mtlAsset = m_Context->Assets->AddMaterial(RandomU64(), "Nimrod");
                mtlAsset->Data.Albedo.x = 0.0f;

                auto camera = createEntt<Entity>();
                camera.Attach<infoComponent>();
                camera.Attach<transformComponent>().Transform.Translate.z = 20.0f;
                camera.Attach<cameraComponent>();

                auto skybox = createEntt<Entity>();
                skybox.Attach<infoComponent>();
                skybox.Attach<skyboxComponent>().Sky = skyboxAsset->UID;
                skybox.Attach<transformComponent>();

                auto light = createEntt<Entity>();
                light.Attach<infoComponent>();
                light.Attach<directLightComponent>().Light.Intensity = 1.0f;
                auto& td = light.Attach<transformComponent>().Transform;
                td.Rotate = glm::vec3(0.0f, 1.0f, -1.0f);

                auto robot = createEntt<Entity>();
                robot.Attach<infoComponent>();
                auto& robotMod = robot.Attach<modelComponent>();
                robotMod.Material = mtlAsset->UID;
                robotMod.Model = robotAsset->UID;
                auto& tr = robot.Attach<transformComponent>().Transform;
                tr.Translate = glm::vec3(0.0f, -14.99f, -15.0f);
                tr.Scale = glm::vec3(0.1f);

                auto plane = createEntt<Entity>();
                plane.Attach<infoComponent>();
                plane.Attach<rigidBodyComponent>().RigidBody.Dynamic = false;
                plane.Attach<colliderComponent>().Collider.Type = ColliderType::BOX;
                auto& planeMod = plane.Attach<modelComponent>();
                planeMod.Material = mtlAsset->UID;
                planeMod.Model = cubeAsset->UID;
                auto& tp = plane.Attach<transformComponent>().Transform;
                tp.Translate = glm::vec3(0.0f, -15.0f, -50.0f);
                tp.Scale = glm::vec3(100.0f, 1.0f, 100.0f);

                for(uint32_t i = 0; i < 10; i++) {
                    auto cube = createEntt<Entity>();
                    cube.Attach<infoComponent>().Name = "Entity" + std::to_string(i);
                    cube.Attach<colliderComponent>().Collider.Type = ColliderType::BOX;
                    // cube.Attach<ScriptComponent>().Script = scriptAsset->UID;
                    cube.Attach<rigidBodyComponent>().RigidBody.Dynamic = true;
                    auto& modelComp = cube.Attach<modelComponent>();
                    modelComp.Material = mtlAsset->UID;
                    modelComp.Model = cubeAsset->UID;
                    auto& tc = cube.Attach<transformComponent>().Transform;
                    tc.Translate = glm::vec3(0.0f, 6.0f * i, -10.0f);
                    tc.Scale *= 5.0f;
                }

                m_Context->Serializer->Serialize(*m_Context->Assets, "resources/projects/assets.yaml");
                m_Context->Serializer->Serialize(m_Context->Scene, "resources/projects/scene.yaml");
            }

            FLOW_INLINE void RenderScene() {
                // shadow map
                enttView<Entity, directLightComponent>([this] (auto light, auto&) {
                   auto& lightDir = light.template Get<transformComponent>().Transform.Rotate;
                   m_Context->Renderer->beginShadowPass(lightDir);

                   enttView<Entity, modelComponent>([this, &lightDir] (auto entity, auto& comp) {
                       auto& transform = entity.template Get<transformComponent>().Transform;
                       auto& model = m_Context->Assets->Get<ModelAsset>(comp.Model);
                       m_Context->Renderer->drawDepth(model.Data, transform);
                   });
                   m_Context->Renderer->endShadowPass();
                });

                // FBO Render
                m_Context->Renderer->newFrame();
                enttView<Entity, cameraComponent>([this] (auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    m_Context->Renderer->setCamera(comp.Camera, transform);
                });

                int32_t lightCounter = 0u;
                enttView<Entity, directLightComponent>([this, &lightCounter] (auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    m_Context->Renderer->setDirectLight(comp.Light, transform, lightCounter);
                    lightCounter++;
                });
                m_Context->Renderer->setDirectLightCount(lightCounter);

                lightCounter = 0;
                enttView<Entity, spotLightComponent>([this, &lightCounter] (auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    m_Context->Renderer->setSpotLight(comp.Light, transform, lightCounter);
                    lightCounter++;
                });
                m_Context->Renderer->setSpotLightCount(lightCounter);

                lightCounter = 0;
                enttView<Entity, pointLightComponent>([this, &lightCounter] (auto entity, auto& comp) {
                   auto& transform = entity.template Get<transformComponent>().Transform;
                   m_Context->Renderer->setPointLight(comp.Light, transform, lightCounter);
                   lightCounter++;
                });
                m_Context->Renderer->setPointLightCount(lightCounter);

                // Model
                enttView<Entity, modelComponent>([this] (auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    auto& material = m_Context->Assets->Get<MaterialAsset>(comp.Material);
                    auto& model = m_Context->Assets->Get<ModelAsset>(comp.Model);

                    // render model
                    m_Context->Renderer->Animate(model.Data, m_Context->deltaTime);
                    m_Context->Renderer->Draw(model.Data, material.Data, transform);
                });

                // Skybox Render
                enttView<Entity, skyboxComponent>([this] (auto entity, auto& comp) {
                   auto& transform = entity.template Get<transformComponent>().Transform;
                   auto& skybox = m_Context->Assets->Get<SkyboxAsset>(comp.Sky);
                   m_Context->Renderer->DrawSkybox(skybox.Data, transform);
                });

                m_Context->Renderer->endFrame();
            }

            FLOW_INLINE void StartScene() {
                CreateEntities();
                // serializer
                m_Context->Serializer->Deserialize(*m_Context->Assets, "resources/projects/assets.yaml");
                m_Context->Serializer->Deserialize(m_Context->Scene, "resources/projects/scene.yaml");

                enttView<Entity, skyboxComponent>([this] (auto entity, auto& comp) {
                    auto& skybox = m_Context->Assets->Get<SkyboxAsset>(comp.Sky);
                    auto skyTex = std::make_shared<texture2D>(skybox.EnvMap);
                    m_Context->Renderer->InitSkybox(skybox.Data, skyTex, skybox.Size);
                });

                enttView<Entity, scriptComponent>([this] (auto entity, auto& comp) {
                    auto& script = m_Context->Assets->Get<ScriptAsset>(comp.Script);
                    auto name = m_Context->Scripts->loadScript(script.Source);
                    m_Context->Scripts->attachScript(entity, name);
                });

                enttView<Entity, rigidBodyComponent>([this] (auto entity, auto& comp) {
                    m_Context->Physics->AddRigidBody(entity);
                });
            }

            FLOW_INLINE void UpdateScene() {
                enttView<Entity, scriptComponent>([this] (auto entity, auto& script) {
                    if (script.Instance) {
                        script.Instance->onUpdate(m_Context->deltaTime);
                    }
                });
                m_Context->Physics->Simulate(1, m_Context->deltaTime);
                enttView<Entity, rigidBodyComponent>([this] (auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    auto pose = comp.RigidBody.actor->getGlobalPose();
                    glm::quat rot(pose.q.x, pose.q.y, pose.q.z, pose.q.w);
                    transform.Rotate = glm::degrees(glm::eulerAngles(rot));
                    transform.Translate = PxToVec3(pose.p);
                });
            }

            FLOW_INLINE void UpdateLayers() {
                for (auto layer : m_Context->Layers) {
                    layer->onUpdate();
                }
                if (m_Context->Layers.size() == 0) {
                    m_Context->Renderer->showFrame();
                }
            }
    };
}
