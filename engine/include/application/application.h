#pragma once
#include "interface.h"

namespace flow {
    struct Application : appInterface
    {
        FLOW_INLINE Application(){
            m_LayerID = TypeID<Application>();
            m_Context = new appContext();

            attachCallback<windowResizeEvent>([this](auto event) {
                m_Context->Renderer->Resize(event.width, event.height);
            });

            RegisterEventCallbacks();
            CreateSceneEntities();
            CreatePhysicsActors();
            CreateSkyboxEnvMaps();
        }

        FLOW_INLINE ~Application()
        {
            DestroyPhysicsActors();
            FLOW_DELETE(m_Context);
        }

        FLOW_INLINE void runContext(){

            while(m_Context->Window->pollEvents()) {
                ComputeFrameDeltaTime();
                RunPhysicsSimulation();
                RenderSceneDepthMap();
                RenderSceneToFBO();
                UpdateAppLayers();
            }
        }

        private:
            // FLOW_INLINE void renderSceneDepth() {

            // }

            FLOW_INLINE void RegisterEventCallbacks() {
                m_Context->Physics->setEventCallback([this](auto e) {
                    // scripting
                });

                attachCallback<windowResizeEvent>([this] (auto e) {
                    m_Context->Renderer->Resize(e.width, e.height);
                });
            }

            FLOW_INLINE void ComputeFrameDeltaTime() {
                static double sLastTime = glfwGetTime();
                double currentTime = glfwGetTime();
                m_Context->deltaTime = currentTime - sLastTime;
                sLastTime = currentTime;
            }
            FLOW_INLINE void RunPhysicsSimulation() {
                m_Context->Physics->Simulate(1, m_Context->deltaTime);

                enttView<Entity, rigidBodyComponent>([this] (auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    const auto& pose = comp.RigidBody.actor->getGlobalPose();
                    glm::quat rot(pose.q.x, pose.q.y, pose.q.z, pose.q.w);
                    transform.Rotate = glm::degrees(glm::eulerAngles(rot));
                    transform.Translate = PxToVec3(pose.p);
                });
            }

            FLOW_INLINE void DestroyPhysicsActors() {
                enttView<Entity, rigidBodyComponent>([this](auto entity, auto& comp) {
                    if (entity.template Has<colliderComponent>()) {
                        auto& collider = entity.template Get<colliderComponent>().Collider;
                        collider.Material->release();
                        collider.Shape->release();
                    }
                    entityID* owner = static_cast<entityID*>(comp.RigidBody.actor->userData);
                    FLOW_DELETE(owner);
                    comp.RigidBody.actor->release();
                });
            }

            FLOW_INLINE void CreateSkyboxEnvMaps() {
                auto skymap = std::make_shared<texture2D>("resources/textures/sky.hdr", true, true);
                enttView<Entity, skyboxComponent>([this, &skymap] (auto entity, auto& comp) {
                    m_Context->Renderer->InitSkybox(comp.Sky, skymap, 2048);
                });
            }

            FLOW_INLINE void RenderSceneDepthMap() {
                enttView<Entity, directLightComponent>([this](auto light, auto&) {
                    auto& lightDir = light.template Get<transformComponent>().Transform.Rotate;
                    m_Context->Renderer->beginShadowPass(lightDir);
                    enttView<Entity, modelComponent>([this, &lightDir] (auto entity, auto& comp) {
                        auto& transform = entity.template Get<transformComponent>().Transform;
                        m_Context->Renderer->drawDepth(comp.Model, transform);
                    });
                    m_Context->Renderer->endShadowPass();
                });
            }

            FLOW_INLINE void CreatePhysicsActors() {
                enttView<Entity, rigidBodyComponent>([this](auto entity, auto& comp) {
                    m_Context->Physics->AddRigidBody(entity);
                });
            }

            FLOW_INLINE void CreateSceneEntities() {
                // load models
                auto walking = std::make_shared<skeletalModel>("resources/models/walking.fbx");
                auto sphereModel = std::make_shared<staticModel>("resources/models/sphere.fbx");
                auto cubeModel = std::make_shared<staticModel>("resources/models/cube.fbx");

                // create scene camera
                auto camera = createEntt<Entity>();
                camera.Attach<transformComponent>().Transform.Translate.z = 20.0f;
                camera.Attach<cameraComponent>();

                // create directional light
                auto light = createEntt<Entity>();
                light.Attach<directLightComponent>().Light.Intensity = 1.0f;
                auto& td = light.Attach<transformComponent>().Transform;
                td.Rotate = glm::vec3(0.0f, 2.0f, -1.0f);

                // create robot entity
                auto robot = createEntt<Entity>();
                robot.Attach<modelComponent>().Model = walking;
                auto& tr = robot.Attach<transformComponent>().Transform;
                tr.Translate = glm::vec3(0.0f, -10.0f, -10.0f);
                tr.Scale = glm::vec3(0.1f);
                robot.Attach<animatorComponent>().Animator = walking->GetAnimator();

                // create plane entity (ground)
                auto plane = createEntt<Entity>();
                plane.Attach<rigidBodyComponent>().RigidBody.Type = RigidBody3D::STATIC;
                plane.Attach<colliderComponent>().Collider.Type = Collider3D::BOX;
                plane.Attach<modelComponent>().Model = cubeModel;
                auto& tp = plane.Attach<transformComponent>().Transform;
                tp.Scale = glm::vec3(100.0f, 1.0f, 100.0f);
                tp.Translate.z = -50.0f;
                tp.Translate.y = -15.0f;

                // create sphere rigid bodies
                for (uint32_t i = 0; i < 50; i++) {
                    Entity sphere = createEntt<Entity>();
                    sphere.Attach<rigidBodyComponent>().RigidBody.Type = RigidBody3D::DYNAMIC;
                    sphere.Attach<colliderComponent>().Collider.Type = Collider3D::SPHERE;
                    sphere.Attach<modelComponent>().Model = sphereModel;
                    auto& ts = sphere.Attach<transformComponent>();
                    ts.Transform.Translate.y = i * 10.0f;
                    ts.Transform.Translate.z = -10.0f;
                    ts.Transform.Translate.x = 3.0f;
                    ts.Transform.Scale *= 5.0f;
                }

                // create cube rigid bodies
                for (uint32_t i = 0; i < 50; i++) {
                    Entity cube = createEntt<Entity>();
                    cube.Attach<rigidBodyComponent>().RigidBody.Type = RigidBody3D::DYNAMIC;
                    cube.Attach<colliderComponent>().Collider.Type = Collider3D::BOX;
                    cube.Attach<modelComponent>().Model = cubeModel;
                    auto& tc = cube.Attach<transformComponent>();
                    tc.Transform.Translate.y = i * 10.0f;
                    tc.Transform.Translate.z = -15.0f;
                    tc.Transform.Rotate.x = i * 10.0f;
                    tc.Transform.Scale *= 5.0f;
                }

                // create skybox entity
                auto skybox = createEntt<Entity>();
                skybox.Attach<transformComponent>();
                skybox.Attach<skyboxComponent>();
            }

            FLOW_INLINE void RenderSceneToFBO() {
                // camera
                m_Context->Renderer->newFrame();
                enttView<Entity, cameraComponent>([this](auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    m_Context->Renderer->setCamera(comp.Camera, transform);
                });

                // point lights
                int32_t lightCounter = 0u;
                enttView<Entity, pointLightComponent>([this, &lightCounter](auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    m_Context->Renderer->setPointLight(comp.Light, transform, lightCounter);
                    lightCounter++;
                });
                m_Context->Renderer->setPointLightCount(lightCounter);

                // spot lights
                lightCounter = 0;
                enttView<Entity, spotLightComponent>([this, &lightCounter](auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    m_Context->Renderer->setSpotLight(comp.Light, transform, lightCounter);
                    lightCounter++;
                });
                m_Context->Renderer->setSpotLightCount(lightCounter);

                // direct lights
                lightCounter = 0;
                enttView<Entity, directLightComponent>([this, &lightCounter](auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    m_Context->Renderer->setDirectLight(comp.Light, transform, lightCounter);
                    lightCounter++;
                });
                m_Context->Renderer->setDirectLightCount(lightCounter);

                enttView<Entity, modelComponent>([this] (auto entity, auto& comp) {
                // compute key frames
                if(entity.template Has<animatorComponent>()) {
                    auto& animator = entity.template Get<animatorComponent>().Animator;
                    auto* transforms = animator->Animate(m_Context->deltaTime);
                    m_Context->Renderer->setJoints(*transforms);
                }

                auto& transform = entity.template Get<transformComponent>().Transform;
                m_Context->Renderer->Draw(comp.Model, comp.Material, transform);
                });

                // skybox rendered last
                enttView<Entity, skyboxComponent>([this](auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    m_Context->Renderer->DrawSkybox(comp.Sky, transform);
                });

                m_Context->Renderer->endFrame();
            }

            FLOW_INLINE void UpdateAppLayers() {
                for (auto layer : m_Context->Layers) {
                    layer->onUpdate();
                }
                m_Context->Renderer->showFrame();
            }
    };
}
