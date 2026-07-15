#pragma once
#include "shader.h"
#include "../utilities/quad.h"

namespace flow {
    struct BRDFShader : public Shader {
        FLOW_INLINE BRDFShader(const std::string& vertFile, const std::string& fragFile)
        : Shader(vertFile, fragFile) {
            m_Quad = createQuad2D();
        }

        FLOW_INLINE uint32_t Generate(int32_t size) {
            uint32_t BRDFMap = 0;
            glCreateTextures(GL_TEXTURE_2D, 1, &BRDFMap);
            glTextureStorage2D(BRDFMap, 1, GL_RG16F, size, size);

            glTextureParameteri(BRDFMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(BRDFMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureParameteri(BRDFMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(BRDFMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            uint32_t FBO = 0;
            glCreateFramebuffers(1, &FBO);
            glNamedFramebufferTexture(FBO, GL_COLOR_ATTACHMENT0, BRDFMap, 0);

            glViewport(0, 0, size, size);
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            glClear(GL_COLOR_BUFFER_BIT);

            Bind();
            m_Quad->Draw(GL_TRIANGLES);
            Unbind();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &FBO);

            return BRDFMap;
        }
        private:
            quad2D m_Quad;
    };
}
