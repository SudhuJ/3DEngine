#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "graphics/shaders/PBR.h"
#include "graphics/shaders/skymap.h"
#include "graphics/shaders/skybox.h"
#include "graphics/shaders/final.h"
#include "graphics/shaders/irradiance.h"
#include "graphics/shaders/BRDF.h"
#include "graphics/shaders/prefiltered.h"
#include "graphics/shaders/shadow.h"
#include "graphics/utilities/data.h"
#include "graphics/utilities/skybox.h"
#include "graphics/models/model.h"
#include "graphics/shaders/bloom.h"
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
            m_Shadow = std::make_unique<ShadowShader>("resources/shaders/shadow.vert", "resources/shaders/shadow.frag");

            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);

            m_SkyMap = std::make_unique<SkyMapShader>("resources/shaders/skymap.vert", "resources/shaders/skymap.frag");
            m_Skybox = std::make_unique<SkyboxShader>("resources/shaders/skybox.vert", "resources/shaders/skybox.frag");
            m_Irrad = std::make_unique<IrradianceShader>("resources/shaders/irradiance.vert", "resources/shaders/irradiance.frag");
            m_Bloom = std::make_unique<BloomShader>("resources/shaders/bloom.vert", "resources/shaders/bloom.frag", width, height);
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
            sky.IrradianceMap = m_Irrad->Generate(sky.m_CubeMap, m_SkyboxMesh, 128);
            sky.PrefilteredMap = m_Prefil->Generate(sky.m_CubeMap, m_SkyboxMesh, 128);
            sky.BRDFMap = m_BRDF->Generate(128);
        }

        FLOW_INLINE void Animate(model3D model, float dt) {
            if (auto joints = model->Animate(dt)) {
                m_PBR->setJoints(*joints);
            }
        }

        FLOW_INLINE void setIBL(Skybox& sky) {
            m_PBR->setEnvMaps(sky.IrradianceMap, sky.BRDFMap, sky.PrefilteredMap, m_Shadow->getDepthmap());
        }

        FLOW_INLINE void processBloom() {
            m_Bloom->Apply(m_Frame->getBrightnessTexture());
        }

        FLOW_INLINE void DrawSkybox(Skybox& sky, transform3D& transform) {
            m_Skybox->Draw(m_SkyboxMesh, sky.m_CubeMap, transform);
            m_PBR->setEnvMaps(sky.IrradianceMap, sky.BRDFMap, sky.PrefilteredMap, m_Shadow->getDepthmap());
        }

        FLOW_INLINE void drawDepth(model3D& model, transform3D& transform) {
            m_Shadow->Draw(model, transform);
        }

        FLOW_INLINE void beginShadowPass(const glm::vec3& lightDir) {
            float nearPlane = 1.0f;
            float farPlane = 10.0f;
            auto proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
            auto view = glm::lookAt(lightDir, glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f));

            auto lightSpaceMatrix = glm::mat4(proj * view);
            m_PBR->Bind();
            m_PBR->setLightSpaceMatrix(lightSpaceMatrix);
            m_Shadow->beginFrame(lightSpaceMatrix);
        }

        FLOW_INLINE void endShadowPass() {
            m_Shadow->endFrame();
        }

        FLOW_INLINE void Resize(int32_t width, int32_t height) {
            m_Frame->Resize(width, height);
            m_Bloom->Resize(width, height);
        }

        FLOW_INLINE void setJoints(std::vector<glm::mat4>& transforms) {
            m_PBR->setJoints(transforms);
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
            processBloom();
            m_Final->Show(m_Frame->getTexture(), m_Bloom->getMap());
        }

        FLOW_INLINE void showFrame() {
            glViewport(0, 0, m_Frame->getWidth(), m_Frame->getHeight());
            m_Final->Show(m_Frame->getTexture(), m_Bloom->getMap());
        }

        FLOW_INLINE void Draw(model3D& model, Material& material, transform3D& transform) {
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
            std::unique_ptr<ShadowShader> m_Shadow;
            std::unique_ptr<BloomShader> m_Bloom;
            skyboxMesh m_SkyboxMesh;
    };
}
