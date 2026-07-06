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

            auto skymap = std::make_shared<texture2D>();
            skymap->Load("resources/textures/sky.hdr", true);

            // load models
            auto sphereModel = std::make_shared<Model>("resources/models/sphere.fbx");
            auto cubeModel = std::make_shared<Model>("resources/models/cube.fbx");

            // create scene camera
            auto camera = createEntt<Entity>();
            camera.Attach<transformComponent>().Transform.Translate.z = 3.0f;
            camera.Attach<cameraComponent>();

            // skybox entity
            auto skybox = createEntt<Entity>();
            skybox.Attach<transformComponent>();
            skybox.Attach<skyboxComponent>();

            // create direct light
            auto dlight = createEntt<Entity>();
            dlight.Attach<directLightComponent>().Light.Intensity = 5.0f;
            auto& stp = dlight.Attach<transformComponent>().Transform;
            stp.Rotate = glm::vec3(0.0f);

            // create sphere entity
            auto sphere = createEntt<Entity>();
            auto& mod = sphere.Attach<modelComponent>();
            mod.Model = sphereModel;
            mod.Material.Emissive = glm::vec3(1.0f);
            mod.Material.Albedo = glm::vec3(0.8f, 0.1f, 0.8f);
            sphere.Attach<transformComponent>().Transform.Translate.x = -1.0f;

            // create cube entity
            auto cube = createEntt<Entity>();
            auto& mod1 = cube.Attach<modelComponent>();
            mod1.Model = cubeModel;
            mod1.Material.Albedo = glm::vec3(0.1f, 0.0f, 0.5f);
            cube.Attach<transformComponent>().Transform.Translate.x = 1.0f;

            enttView<Entity, skyboxComponent>([this, &skymap] (auto entity, auto& comp) {
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
                    enttView<Entity, modelComponent>([this, &lightDir] (auto entity, auto& comp) {
                        auto& transform = entity.template Get<transformComponent>().Transform;
                        m_Context->Renderer->drawDepth(comp.Model, transform);
                    });
                    m_Context->Renderer->endShadowPass();
                });
            }
    };
}
