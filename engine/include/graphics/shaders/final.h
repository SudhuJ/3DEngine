#pragma once
#include "shader.h"
#include "../utilities/quad.h"

namespace flow {
    struct finalShader : public Shader {
        FLOW_INLINE finalShader(const std::string& vertFile, const std::string& fragFile, int32_t width, int32_t height)
            : Shader(vertFile, fragFile) {
            u_Map = glGetUniformLocation(m_FragmentProgID, "u_map");
            u_Bloom = glGetUniformLocation(m_FragmentProgID, "u_bloom");
            CreateBuffer(width, height);
            m_Quad = createQuad2D();
        }

        FLOW_INLINE ~finalShader() {
            glDeleteTextures(1, &m_Final);
            glDeleteFramebuffers(1, &m_FBO);
        }

        FLOW_INLINE void Render(uint32_t map, uint32_t bloom) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            Bind();
            glBindTextureUnit(0, map);
            glProgramUniform1i(m_FragmentProgID, u_Map, 0);

            glBindTextureUnit(1, bloom);
            glProgramUniform1i(m_FragmentProgID, u_Bloom, 1);

            m_Quad->Draw(GL_TRIANGLES);
            Unbind();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        FLOW_INLINE void Show(uint32_t map, uint32_t bloom) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            Bind();
            glBindTextureUnit(0, map);
            glProgramUniform1i(m_FragmentProgID, u_Map, 0);

            glBindTextureUnit(1, bloom);
            glProgramUniform1i(m_FragmentProgID, u_Bloom, 1);

            m_Quad->Draw(GL_TRIANGLES);
            Unbind();
        }

        FLOW_INLINE void Resize(int32_t width, int32_t height) {

            if (width < 1 || height < 1) return;

            glDeleteTextures(1, &m_Final);
            glCreateTextures(GL_TEXTURE_2D, 1, &m_Final);
            glTextureStorage2D(m_Final, 1, GL_RGBA16F, width, height);
            glTextureParameteri(m_Final, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(m_Final, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_Final, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_Final, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glNamedFramebufferTexture(m_FBO, GL_COLOR_ATTACHMENT0, m_Final, 0);
        }

        FLOW_INLINE uint32_t getMap() {
            return m_Final;
        }

    private:
        FLOW_INLINE void CreateBuffer(int32_t width, int32_t height) {
            glCreateFramebuffers(1, &m_FBO);
            glCreateTextures(GL_TEXTURE_2D, 1, &m_Final);
            glTextureStorage2D(m_Final, 1, GL_RGBA16F, width, height);
            glTextureParameteri(m_Final, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(m_Final, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_Final, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_Final, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glNamedFramebufferTexture(m_FBO, GL_COLOR_ATTACHMENT0, m_Final, 0);

            if (glCheckNamedFramebufferStatus(m_FBO, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                FLOW_ERROR("glCheckNamedFramebufferStatus() Failed!");
            }
        }

    private:
        uint32_t m_Final = 0u;
        uint32_t m_FBO = 0u;
        uint32_t u_Map = 0u;
        int32_t u_Bloom = 0u;

        quad2D m_Quad;
    };
}
