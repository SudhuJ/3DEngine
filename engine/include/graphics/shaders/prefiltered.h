#pragma once
#include "shader.h"
#include "../utilities/skybox.h"
#include "../textures/texture.h"

namespace flow {
    struct PrefilteredShader : public Shader {
        FLOW_INLINE PrefilteredShader(const std::string& vertFile, const std::string& fragFile)
        : Shader(vertFile, fragFile) {
            u_view = glGetUniformLocation(m_VertexProgID, "u_view");
            u_proj = glGetUniformLocation(m_VertexProgID, "u_proj");
            u_CubeMap = glGetUniformLocation(m_FragmentProgID, "u_cubemap");
            u_Roughness = glGetUniformLocation(m_FragmentProgID, "u_roughness");
        }

        FLOW_INLINE uint32_t Generate(uint32_t skyCubeMap, skyboxMesh& mesh, int32_t size) {
            glm::mat4 views[6] = {
                glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
                };

            glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

            uint32_t prefilteredMap = 0;
            int32_t maxMipLevels = 5;

            glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &prefilteredMap);
            glTextureStorage2D(prefilteredMap, maxMipLevels, GL_RGB16F, size, size);

            glTextureParameteri(prefilteredMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(prefilteredMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureParameteri(prefilteredMap, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTextureParameteri(prefilteredMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(prefilteredMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            uint32_t FBO = 0, RBO = 0;

            glCreateFramebuffers(1, &FBO);
            glCreateRenderbuffers(1, &RBO);
            glNamedFramebufferRenderbuffer(FBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

            Bind();
            glProgramUniformMatrix4fv(m_VertexProgID, u_proj, 1, GL_FALSE, glm::value_ptr(proj));
            glBindTextureUnit(0, skyCubeMap);
            glProgramUniform1i(m_FragmentProgID, u_CubeMap, 0);

            for (int32_t mip = 0; mip < maxMipLevels; mip++) {
                int32_t mipSize = static_cast<int32_t>(size * std::pow(0.5f, mip));
                float roughness = static_cast<float>(mip) / static_cast<float>(maxMipLevels - 1);

                glProgramUniform1f(m_FragmentProgID, u_Roughness, roughness);
                glViewport(0, 0, mipSize, mipSize);
                glNamedRenderbufferStorage(RBO, GL_DEPTH_COMPONENT24, mipSize, mipSize);

                for (uint32_t i = 0; i < 6; i++) {
                    glProgramUniformMatrix4fv(m_VertexProgID, u_view, 1, GL_FALSE, glm::value_ptr(views[i]));
                    glNamedFramebufferTextureLayer(FBO, GL_COLOR_ATTACHMENT0, prefilteredMap, mip, i);
                    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    renderSkyboxMesh(mesh);
                }
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            Unbind();

            glDeleteRenderbuffers(1, &RBO);
            glDeleteFramebuffers(1, &FBO);

            return prefilteredMap;
        }

        private:
            int32_t u_Roughness = 0;
            int32_t u_view = 0;
            int32_t u_proj = 0;
            int32_t u_CubeMap = 0;
    };
}
