#pragma once
#include "shader.h"
#include "../utilities/quad.h"
#include "../utilities/data.h"
#include "../models/model.h"

namespace flow {
    struct ShadowShader : public Shader {
        FLOW_INLINE ShadowShader(const std::string& vertFile, const std::string& fragFile)
        : Shader(vertFile, fragFile) {
            u_Model = glGetUniformLocation(m_VertexProgID, "u_model");
            u_lightSpace = glGetUniformLocation(m_VertexProgID, "u_lightSpace");

            glCreateTextures(GL_TEXTURE_2D, 1, &m_Depthmap);
            glTextureStorage2D(m_Depthmap, 1, GL_DEPTH_COMPONENT24, m_mapSize, m_mapSize);
            glTextureParameteri(m_Depthmap, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(m_Depthmap, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTextureParameteri(m_Depthmap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTextureParameteri(m_Depthmap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float border[] = {1.0f, 1.0f, 1.0f, 1.0f};
            glTextureParameterfv(m_Depthmap, GL_TEXTURE_BORDER_COLOR, border);

            glCreateFramebuffers(1, &m_Framebuffer);
            glNamedFramebufferTexture(m_Framebuffer, GL_DEPTH_ATTACHMENT, m_Depthmap, 0);
            glNamedFramebufferDrawBuffer(m_Framebuffer, GL_NONE);
            glNamedFramebufferReadBuffer(m_Framebuffer, GL_NONE);
        }

        FLOW_INLINE ~ShadowShader() {
            glDeleteFramebuffers(1, &m_Framebuffer);
            glDeleteTextures(1, &m_Depthmap);
        }

        FLOW_INLINE void Draw(model3D& model, transform3D& transform) {
            glProgramUniformMatrix4fv(m_VertexProgID, u_Model, 1, GL_FALSE, glm::value_ptr(transform.Matrix()));
            model->Draw(GL_TRIANGLES);
        }

        FLOW_INLINE void beginFrame(const glm::mat4& lightSpace) {
            Bind();
            glProgramUniformMatrix4fv(m_VertexProgID, u_lightSpace, 1, GL_FALSE, glm::value_ptr(lightSpace));
            glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
            glViewport(0, 0, m_mapSize, m_mapSize);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glCullFace(GL_FRONT); // reduces peter-panning
        }

        FLOW_INLINE void endFrame() {
            glDisable(GL_DEPTH_TEST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        FLOW_INLINE uint32_t getDepthmap() {
            return m_Depthmap;
        }

        private:
            uint32_t m_Framebuffer = -1;
            uint32_t m_Depthmap = -1;

            int32_t m_mapSize = 2048;
            int32_t u_lightSpace = 0u;
            int32_t u_Model = 0u;
    };
}
