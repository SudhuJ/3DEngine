#pragma once
#include "interface.h"

namespace flow {
    struct Application : appInterface
    {
        FLOW_INLINE Application(engineMode mode = engineMode::EDITOR,
            const std::string& projectPath = ""){
            m_LayerID = TypeID<Application>();
            m_Context = new appContext();
            m_Context->Mode = mode;
            m_StartupProjectPath = projectPath;

            RegisterCallbacks();
            StartScene();
        }

        FLOW_INLINE ~Application() {
            for (auto layer : m_Context->Layers){
                FLOW_DELETE(layer);
            }
            FLOW_DELETE(m_Context);
        }

        FLOW_INLINE void runContext(bool showFrame = true) {
            while(m_Context->Window->pollEvents()) {
                UpdateDeltaTime();
                if (glfwGetKey(GetWindowHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                    if (m_Context->Mode == engineMode::PLAY_IN_EDITOR)
                        m_Context->StopRuntime();
                    else if (m_Context->Mode == engineMode::STANDALONE_GAME)
                        glfwSetWindowShouldClose(GetWindowHandle(), true);
                }
                UpdateScene();
                RenderScene();
                UpdateLayers(showFrame);
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
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    glViewport(0, 0, e.width, e.height);
                    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT);
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

                attachCallback<mouseMotionEvent>([this] (auto e) {
                    enttView<Entity, scriptComponent>([e] (auto entity, auto& script) {
                        if (script.Instance) {
                            script.Instance->onMouseMove(e.targetX, e.targetY);
                        }
                    });
                });

                attachCallback<mouseDragEvent>([this] (auto e) {
                    enttView<Entity, scriptComponent>([e] (auto entity, auto& script) {
                        if (script.Instance) {
                            script.Instance->onMouseDrag(e.deltaX, e.deltaY);
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

            FLOW_INLINE void SpawnDefaultScene() {
                // Skybox
                auto skyAsset = m_Context->Assets->AddSkybox(RandomU64(), "resources/textures/sky.hdr", 2048);
                auto skyEnt = createEntt<Entity>();
                skyEnt.Attach<infoComponent>().Name = "Sky";
                skyEnt.Attach<skyboxComponent>().Sky = skyAsset->UID;
                skyEnt.Attach<transformComponent>();

                // Textured ground plane
                auto planeModel = m_Context->Assets->AddModel(RandomU64(), "resources/models/cube.fbx");
                // auto planeTex   = m_Context->Assets->AddTexture(RandomU64(), "resources/placeholders/textures/PNG/Dark/texture_01.png");
                auto planeMat   = m_Context->Assets->AddMaterial(RandomU64(), "Ground");
                planeMat->Data.Albedo = glm::vec4(0.25f, 0.0f, 1.0f, 1.0f);
                // planeMat->Data.AlbedoMap = planeTex->Data;
                // planeMat->AlbedoMap      = planeTex->UID;

                auto plane = createEntt<Entity>();
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
                auto sun = createEntt<Entity>();
                sun.Attach<infoComponent>().Name = "Sun";
                auto& sunLight = sun.Attach<directLightComponent>().Light;
                sunLight.Intensity = 2.5f;
                sunLight.Radiance  = glm::vec3(2.5f);
                auto& sunT = sun.Attach<transformComponent>().Transform;
                sunT.Rotate = glm::vec3(-60.0f, 0.0f, 0.0f);

                // Movement + PlayerCharacter
                auto scriptAsset = m_Context->Assets->AddScript(RandomU64(), "resources/scripts/testScript.lua");
                auto player = createEntt<Entity>();
                player.Attach<infoComponent>();
                player.Attach<cameraComponent>();
                player.Attach<charControllerComponent>();
                player.Attach<scriptComponent>().Script = scriptAsset->UID;
                auto& playerCC = player.Attach<charControllerComponent>().Controller;
                playerCC.SpawnPosition = glm::vec3(0.0f, 0.0f, 0.0f);
                auto& playerT = player.Attach<transformComponent>().Transform;
                playerT.Translate = playerCC.SpawnPosition;
            }

            FLOW_INLINE void RenderScene() {
                m_Context->Renderer->setAmbient(glm::vec3(0.0f));

                // shadow map
                enttView<Entity, directLightComponent>([this] (auto light, auto&) {
                    auto& transform = light.template Get<transformComponent>().Transform;
                    glm::quat rotation = glm::quat(glm::radians(transform.Rotate));
                    glm::vec3 forward = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
                    m_Context->Renderer->beginShadowPass(forward);
                });

                enttView<Entity, modelComponent>([this] (auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    auto& model = m_Context->Assets->Get<ModelAsset>(comp.Model);
                    m_Context->Renderer->drawDepth(model.Data, transform);
                });
                m_Context->Renderer->endShadowPass();


                // FBO Render
                m_Context->Renderer->newFrame();
                enttView<Entity, cameraComponent>([this] (auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    m_Context->Renderer->setCamera(comp.Camera, transform);
                });
                enttView<Entity, skyboxComponent>([this] (auto entity, auto& comp) {
                    auto& skybox = m_Context->Assets->Get<SkyboxAsset>(comp.Sky);
                    m_Context->Renderer->setIBL(skybox.Data);
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

            FLOW_INLINE void InitSkybox() {
                enttView<Entity, skyboxComponent>([this] (auto entity, auto& comp) {
                    auto& skybox = m_Context->Assets->Get<SkyboxAsset>(comp.Sky);
                    auto skyTex = std::make_shared<texture2D>(skybox.EnvMap);
                    m_Context->Renderer->InitSkybox(skybox.Data, skyTex, skybox.Size);
                });
            }

            FLOW_INLINE void InitRuntime() {
                enttView<Entity, scriptComponent>([this] (auto entity, auto& comp) {
                    auto& script = m_Context->Assets->Get<ScriptAsset>(comp.Script);
                    auto name = m_Context->Scripts->loadScript(script.Source);
                    m_Context->Scripts->attachScript(entity, name);
                });

                enttView<Entity, rigidBodyComponent>([this] (auto entity, auto& comp) {
                    m_Context->EnsureMeshCollider(entity);
                    m_Context->Physics->AddRigidBody(entity);
                });

                enttView<Entity, charControllerComponent>([this] (auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    transform.Translate = comp.Controller.SpawnPosition;
                    comp.Controller.Yaw = comp.Controller.SpawnYaw;
                    comp.Controller.Pitch = 0.0f;
                    m_Context->Physics->AddCharacterController(entity);
                });
            }

            FLOW_INLINE void InitSceneRuntime() {
                InitSkybox();
                InitRuntime();
            }

            FLOW_INLINE void StartScene() {

                auto* project = m_Context->Project->Current.get();
                if (!project && !m_StartupProjectPath.empty()) {
                    project = m_Context->Project->OpenProject(m_StartupProjectPath);
                }
                if (project) {
                    if (std::filesystem::exists(project->AssetsPath())) {
                        m_Context->Serializer->Deserialize(*m_Context->Assets, project->AssetsPath());
                        m_Context->Assets->ResolveMaterialTextures();
                    }
                    if (std::filesystem::exists(project->ScenePath())) {
                        m_Context->Serializer->Deserialize(m_Context->Scene, project->ScenePath());
                    }
                }
                if (m_Context->Scene.storage<entityID>().empty()) {
                    SpawnDefaultScene();
                }
                if (m_Context->Mode == engineMode::STANDALONE_GAME) {
                    InitSceneRuntime();
                    glfwSetInputMode(GetWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
                else {
                    InitSkybox();

                }
            }

            FLOW_INLINE void UpdateScene() {
                enttView<Entity, scriptComponent>([this] (auto entity, auto& script) {
                    if (script.Instance) {
                        script.Instance->onUpdate(m_Context->deltaTime);
                    }
                });

                if (m_Context->Mode != engineMode::EDITOR) {
                    enttView<Entity, charControllerComponent>([this] (auto entity, auto& comp) {
                        auto& transform = entity.template Get<transformComponent>().Transform;
                        auto* win = GetWindowHandle();

                        // --- Read movement intent from script, oriented by yaw ---
                        glm::vec3 move = comp.Controller.MoveIntent;
                        comp.Controller.MoveIntent = {0.0f, 0.0f, 0.0f};

                        if (glm::length(move) > 0.0f) {
                            glm::vec3 forwardDir(-glm::sin(comp.Controller.Yaw), 0.0f, -glm::cos(comp.Controller.Yaw));
                            glm::vec3 rightDir(glm::cos(comp.Controller.Yaw), 0.0f, -glm::sin(comp.Controller.Yaw));
                            move = (forwardDir * move.z + rightDir * move.x)
                                 * comp.Controller.MoveSpeed * static_cast<float>(m_Context->deltaTime);
                        }
                        else {
                            move = glm::vec3(0.0f);
                        }

                        // --- Apply gravity to vertical velocity ---
                        float dt = static_cast<float>(m_Context->deltaTime);
                        comp.Controller.VerticalVelocity += -9.81f * dt;
                        move.y += comp.Controller.VerticalVelocity * dt;

                        // --- Move the controller ---
                        if (comp.ControllerPtr) {
                            PxControllerFilters filters;
                            PxU32 collisionFlags = comp.ControllerPtr->move(Vec3ToPx(move), 0.001f, dt, filters);

                            if (collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN) {
                                comp.Controller.VerticalVelocity = 0.0f;
                            }

                            PxExtendedVec3 footPos = comp.ControllerPtr->getPosition();

                            transform.Translate = glm::vec3(
                                static_cast<float>(footPos.x),
                                static_cast<float>(footPos.y),
                                static_cast<float>(footPos.z)
                            );
                        }

                        // --- Apply eye offset + pitch + yaw for the camera ---
                        transform.Translate.y += comp.Controller.EyeOffset;
                        transform.Rotate.x = glm::degrees(comp.Controller.Pitch);
                        transform.Rotate.y = glm::degrees(comp.Controller.Yaw);
                        transform.Rotate.z = 0.0f;
                    });
                }

                m_Context->Physics->Simulate(1, m_Context->deltaTime);

                enttView<Entity, rigidBodyComponent>([this] (auto entity, auto& comp) {
                    if (!comp.RigidBody.actor) return;
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    auto pose = comp.RigidBody.actor->getGlobalPose();
                    glm::quat rot(pose.q.x, pose.q.y, pose.q.z, pose.q.w);
                    transform.Rotate = glm::degrees(glm::eulerAngles(rot));
                    transform.Translate = PxToVec3(pose.p);
                });
            }

            FLOW_INLINE void UpdateLayers(bool showFrame) {
                for (auto layer : m_Context->Layers) {
                    layer->onUpdate();
                }
                m_Context->Renderer->showFrame(showFrame);
            }

        private:
            std::string m_StartupProjectPath;
    };
}
