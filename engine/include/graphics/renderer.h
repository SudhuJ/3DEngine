#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "window/events.h"

#include "graphics/shaders/PBR.h"
#include "graphics/shaders/skymap.h"
#include "graphics/shaders/skybox.h"
#include "graphics/shaders/final.h"
#include "graphics/shaders/irradiance.h"
#include "graphics/shaders/BRDF.h"
#include "graphics/shaders/prefiltered.h"

#include "buffers/frame.h"


namespace flow {
    struct graphicsRenderer {
        FLOW_INLINE graphicsRenderer(int32_t width, int32_t height) {
            if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
            {
                FLOW_CRITICAL("Failed to initialize GLAD.");
                exit(EXIT_FAILURE);
            }
            m_Frame = std::make_unique<frameBuffer>(width, height);
            m_Final = std::make_unique<finalShader>("resources/shaders/final.vert", "resources/shaders/final.frag");
            m_PBR = std::make_unique<PBRShader>("resources/shaders/pbr.vert", "resources/shaders/pbr.frag");
            m_Prefil = std::make_unique<PrefilteredShader>("resources/shaders/prefiltered.vert", "resources/shaders/prefiltered.frag");
            m_BRDF = std::make_unique<BRDFShader>("resources/shaders/brdf.vert", "resources/shaders/brdf.frag");

            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

            m_SkyMap = std::make_unique<SkyMapShader>("resources/shaders/skymap.vert", "resources/shaders/skymap.frag");
            m_Skybox = std::make_unique<SkyboxShader>("resources/shaders/skybox.vert", "resources/shaders/skybox.frag");
            m_Irrad = std::make_unique<IrradianceShader>("resources/shaders/irradiance.vert", "resources/shaders/irradiance.frag");
            m_SkyboxMesh = createSkyboxMesh();
        }

        FLOW_INLINE void setCamera(camera3D& camera, transform3D& transform) {
            m_PBR->setCamera(camera, transform, m_Frame->Ratio());
            m_Skybox->setCamera(camera, transform, m_Frame->Ratio());
            m_PBR->Bind();
        }

        FLOW_INLINE void Draw(mesh3D& mesh, transform3D& transform) {
            m_PBR->Draw(mesh, transform);
        }

        FLOW_INLINE void InitSkybox(Skybox& sky, Texture& texture, int32_t size) {
            sky.m_CubeMap = m_SkyMap->Generate(texture, m_SkyboxMesh, size);
            sky.IrradianceMap = m_Irrad->Generate(texture, m_SkyboxMesh, 32);
            sky.PrefilteredMap = m_Prefil->Generate(texture, m_SkyboxMesh, size);
            sky.BRDFMap = m_BRDF->Generate(size);
        }

        FLOW_INLINE void DrawSkybox(Skybox& sky, transform3D& transform) {
            m_Skybox->Draw(m_SkyboxMesh, sky.m_CubeMap, transform);
            m_PBR->setEnvMaps(sky.IrradianceMap, sky.BRDFMap, sky.PrefilteredMap);
        }

        FLOW_INLINE void Resize(int32_t width, int32_t height) {
            m_Frame->Resize(width, height);
        }

        FLOW_INLINE uint32_t getFrame() {
            return m_Frame->getTexture();
        }

        FLOW_INLINE void newFrame() {
            m_Frame->Begin();
            m_PBR->Bind();
        }

        FLOW_INLINE void endFrame() {
            m_PBR->Unbind();
            m_Frame->End();
        }

        FLOW_INLINE void showFrame() {
            m_Final->Show(m_Frame->getTexture());
        }

        FLOW_INLINE void Draw(model3D& model, PBRMaterial& material, transform3D& transform) {
            m_PBR->Draw(model, material, transform);
        }

        FLOW_INLINE void setPointLight(pointLight& light, transform3D& transform, int32_t index) {
            m_PBR->setPointLight(light, transform, index);
        }

        FLOW_INLINE void setPointLightCount(int32_t count) {
            m_PBR->setPointLightCount(count);
        }

        FLOW_INLINE void setDirectLight(directLight& light, transform3D& transform, int32_t index) {
            m_PBR->setDirectLight(light, transform, index);
        }

        FLOW_INLINE void setDirectLightCount(int32_t count) {
            m_PBR->setDirectLightCount(count);
        }

        FLOW_INLINE void setSpotLight(spotLight& light, transform3D& transform, int32_t index) {
            m_PBR->setSpotLight(light, transform, index);
        }

        FLOW_INLINE void setSpotLightCount(int32_t count) {
            m_PBR->setSpotLightCount(count);
        }

        private:
            std::unique_ptr<finalShader> m_Final;
            std::unique_ptr<frameBuffer> m_Frame;
            std::unique_ptr<PBRShader> m_PBR;

            std::unique_ptr<SkyMapShader> m_SkyMap;
            std::unique_ptr<SkyboxShader> m_Skybox;
            std::unique_ptr<IrradianceShader> m_Irrad;
            std::unique_ptr<BRDFShader> m_BRDF;
            std::unique_ptr<PrefilteredShader> m_Prefil;
            skyboxMesh m_SkyboxMesh;
    };
}
