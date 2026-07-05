#pragma once
#include "shader.h"
#include "../utilities/skybox.h"
#include "../textures/texture.h"

namespace flow {
    struct SkyMapShader : public Shader {
        FLOW_INLINE SkyMapShader(const std::string& vertFile, const std::string& fragFile)
        : Shader(vertFile, fragFile) {
            u_view = glGetUniformLocation(m_VertexProgID, "u_view");
            u_proj = glGetUniformLocation(m_VertexProgID, "u_proj");
            u_map = glGetUniformLocation(m_FragmentProgID, "u_map");
        }

        FLOW_INLINE uint32_t Generate(Texture& texture, skyboxMesh& mesh, int32_t size) {
            glm::mat4 views[6] = {
                glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
                };

            glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

            uint32_t cubeMap = -1;

            glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubeMap);
            glTextureStorage2D(cubeMap, 1, GL_RGB16F, size, size);

            glTextureParameteri(cubeMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(cubeMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureParameteri(cubeMap, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTextureParameteri(cubeMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(cubeMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            uint32_t FBO = -1, RBO = -1;

            glCreateFramebuffers(1, &FBO);
            glCreateRenderbuffers(1, &RBO);
            glNamedRenderbufferStorage(RBO, GL_DEPTH_COMPONENT24, size, size);
            glNamedFramebufferRenderbuffer(FBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

            Bind();
            glProgramUniformMatrix4fv(m_VertexProgID, u_proj, 1, GL_FALSE, glm::value_ptr(proj));
            texture->Use(m_FragmentProgID, u_map, 0);
            glViewport(0, 0, size, size);

            for (uint32_t i = 0; i < 6; i++) {
                glProgramUniformMatrix4fv(m_VertexProgID, u_view, 1, GL_FALSE, glm::value_ptr(views[i]));
                glNamedFramebufferTextureLayer(FBO, GL_COLOR_ATTACHMENT0, cubeMap, 0, i);
                glBindFramebuffer(GL_FRAMEBUFFER, FBO);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                renderSkyboxMesh(mesh);
            }

            glGenerateTextureMipmap(cubeMap);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            Unbind();

            glDeleteRenderbuffers(1, &RBO);
            glDeleteFramebuffers(1, &FBO);

            return cubeMap;
        }

        private:
            int32_t u_view = -1;
            int32_t u_proj = -1;
            int32_t u_map = -1;
    };
}
