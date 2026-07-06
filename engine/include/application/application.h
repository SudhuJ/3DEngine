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
        }

        FLOW_INLINE ~Application()
        {
            FLOW_DELETE(m_Context);
        }

        FLOW_INLINE void runContext(){
            // load textures
            auto albedo = std::make_shared<texture2D>("resources/textures/Metal027_1K-JPG/Color.jpg");
            auto roughness = std::make_shared<texture2D>("resources/textures/Metal027_1K-JPG/Roughness.jpg");
            auto metallic = std::make_shared<texture2D>("resources/textures/Metal027_1K-JPG/Metalness.jpg");
            auto normal = std::make_shared<texture2D>("resources/textures/Metal027_1K-JPG/NormalGL.jpg");

            // load models
            auto sphereModel = std::make_shared<Model>("resources/models/sphere.fbx");

            // create scene camera
            auto camera = createEntt<Entity>();
            camera.Attach<transformComponent>().Transform.Translate.z = 3.0f;
            camera.Attach<cameraComponent>();
            FLOW_INFO("camera created");
            // skybox
            auto skymap = std::make_shared<texture2D>();
            skymap->Load("resources/textures/qwantani_dusk_2_puresky_1k.hdr", true);
            auto skybox = createEntt<Entity>();
            skybox.Attach<transformComponent>();
            skybox.Attach<skyboxComponent>();

            // create direct light
            auto dlight = createEntt<Entity>();
            dlight.Attach<directLightComponent>();
            auto& dtp = dlight.Attach<transformComponent>().Transform;
            dtp.Rotate = glm::vec3(0.0f, 0.0f, -1.0f);
            dtp.Translate.z = -1.0f;
            FLOW_INFO("direct light created");

            // create spot light
            auto slight = createEntt<Entity>();
            auto& slComponent = slight.Attach<spotLightComponent>();
            slComponent.Light.Radiance = glm::vec3(1.0f, 1.0f, 1.0f); // Pure white light
            slComponent.Light.Intensity = 15.0f;                      // High intensity to see it easily

            // FIX: Set explicit cone angles (in cosines or degrees, matching what your struct expects)
            // Assuming standard cosines: Inner cone 15 degrees, Outer cone 25 degrees
            slComponent.Light.Cutoff = 15.0f;
            slComponent.Light.Falloff = 25.0f;

            auto& sp = slight.Attach<transformComponent>().Transform;
            // Position the spotlight slightly above and in front of the object, looking down at it
            sp.Translate = glm::vec3(0.0f, 2.0f, 2.0f);
            sp.Rotate = glm::vec3(-45.0f, 0.0f, 0.0f); // Tilted 45 degrees downward


            // create entity with model
            auto cube = createEntt<Entity>();
            cube.Attach<transformComponent>().Transform.Translate.z = 0.0f;
            auto& mod = cube.Attach<modelComponent>();
            mod.Model = sphereModel;

            mod.Material.Albedo   = glm::vec3(1.0f);
            mod.Material.Metallic = 0.5f;
            mod.Material.Roughness = 0.25f;

            mod.Material.RoughnessMap = roughness;
            mod.Material.AlbedoMap = albedo;
            mod.Material.NormalMap = normal;

            enttView<Entity, skyboxComponent>([this, &skymap] (auto entity, auto& comp)
            {
                m_Context->Renderer->InitSkybox(comp.Sky, skymap, 2048);
                m_Context->Renderer->setIBL(comp.Sky);
            });

            while(m_Context->Window->pollEvents()){
                renderSceneDepth();
                m_Context->Renderer->newFrame();

                int32_t lightCounter = 0;
                enttView<Entity, pointLightComponent>([this, &lightCounter](auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    m_Context->Renderer->setPointLight(comp.Light, transform, lightCounter);
                    lightCounter++;
                });
                m_Context->Renderer->setPointLightCount(lightCounter);

                lightCounter = 0;
                enttView<Entity, directLightComponent>([this, &lightCounter](auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    m_Context->Renderer->setDirectLight(comp.Light, transform, lightCounter);
                    lightCounter++;
                });
                m_Context->Renderer->setDirectLightCount(lightCounter);

                lightCounter = 0;
                enttView<Entity, spotLightComponent>([this, &lightCounter](auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    m_Context->Renderer->setSpotLight(comp.Light, transform, lightCounter);
                    lightCounter++;
                    transform.Rotate.y += 1.0f;
                });
                m_Context->Renderer->setSpotLightCount(lightCounter);

                // set shader cam
                enttView<Entity, cameraComponent>([this](auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    m_Context->Renderer->setCamera(comp.Camera, transform);
                    // transform.Rotate.y += 1.0f;
                });

                enttView<Entity, modelComponent>([this](auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    m_Context->Renderer->Draw(comp.Model, comp.Material, transform);
                    transform.Rotate.y += 1.0f;
                });


                enttView<Entity, skyboxComponent>([this](auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    m_Context->Renderer->DrawSkybox(comp.Sky, transform);
                    transform.Rotate.y += 0.15f;
                });

                m_Context->Renderer->endFrame();

                for(auto layer : m_Context->Layers){
                    layer->onUpdate();
                }

                m_Context->Renderer->showFrame();
            }
        }

        private:
            FLOW_INLINE void renderSceneDepth() {
                enttView<Entity, directLightComponent>([this](auto light, auto&) {
                    auto& lightDir = light.template Get<transformComponent>().Transform.Rotate;
                    m_Context->Renderer->beginShadowPass(lightDir);
                    enttView<Entity, modelComponent>([this, &lightDir] (auto entity, auto& comp)
                    {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    m_Context->Renderer->drawDepth(comp.Model, transform);
                    });
                    m_Context->Renderer->endShadowPass();
                });
            }
    };
}
