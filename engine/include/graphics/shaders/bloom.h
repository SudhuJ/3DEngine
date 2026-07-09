#pragma once
#include "shader.h"
#include "../utilities/skybox.h"
#include "../textures/texture.h"
#include "../utilities/quad.h"

namespace flow {
    struct BloomShader : public Shader {
        FLOW_INLINE BloomShader(const std::string& vertFile, const std::string& fragFile,
            int32_t width, int32_t height): Shader(vertFile, fragFile), m_Width(width), m_Height(height)  {
            u_brightnessMap = glGetUniformLocation(m_FragmentProgID, "u_brightnessMap");
            u_horizontalPass = glGetUniformLocation(m_FragmentProgID, "u_horizontalPass");
            u_frameWidth = glGetUniformLocation(m_FragmentProgID, "u_frameWidth");
            u_frameHeight = glGetUniformLocation(m_FragmentProgID, "u_frameHeight");

            m_Quad = createQuad2D();

            m_Height = height / m_Scale;
            m_Width = width / m_Scale;

            glCreateFramebuffers(1, &m_FrameBuffer[0]);
            glCreateFramebuffers(1, &m_FrameBuffer[1]);

            glCreateTextures(GL_TEXTURE_2D, 1, &m_PingPongMaps[0]);
            glCreateTextures(GL_TEXTURE_2D, 1, &m_PingPongMaps[1]);

            for (auto i = 0; i < 2; i++) {
                glTextureStorage2D(m_PingPongMaps[i], 1, GL_RGB16F, m_Width, m_Height);
                glTextureParameteri(m_PingPongMaps[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTextureParameteri(m_PingPongMaps[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTextureParameteri(m_PingPongMaps[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTextureParameteri(m_PingPongMaps[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glNamedFramebufferTexture(m_FrameBuffer[i], GL_COLOR_ATTACHMENT0, m_PingPongMaps[i], 0);
            }
        }

        FLOW_INLINE void Resize(int32_t width, int32_t height) {
            m_Width = width / m_Scale;
            m_Height = height / m_Scale;
            for (auto i = 0; i < 2; i++) {
                glDeleteTextures(1, &m_PingPongMaps[i]);
                glCreateTextures(GL_TEXTURE_2D, 1, &m_PingPongMaps[i]);
                glTextureStorage2D(m_PingPongMaps[i], 1, GL_RGB16F, m_Width, m_Height);
                glTextureParameteri(m_PingPongMaps[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTextureParameteri(m_PingPongMaps[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTextureParameteri(m_PingPongMaps[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTextureParameteri(m_PingPongMaps[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glNamedFramebufferTexture(m_FrameBuffer[i], GL_COLOR_ATTACHMENT0, m_PingPongMaps[i], 0);
            }
        }

        FLOW_INLINE uint32_t getMap() const {
            return m_PingPongMaps[0];
        }

        FLOW_INLINE void Apply(uint32_t brightnessTexture) {
            Bind();
            glBindTextureUnit(0, brightnessTexture);
            glProgramUniform1i(m_FragmentProgID, u_brightnessMap, 0);
            glProgramUniform1i(m_FragmentProgID, u_frameWidth, m_Width);
            glProgramUniform1i(m_FragmentProgID, u_frameHeight, m_Height);
            glViewport(0, 0, m_Width, m_Height);

            bool horizontal = true;
            for (uint32_t i = 0; i < 10; i++) {
                glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer[horizontal]);
                glProgramUniform1i(m_FragmentProgID, u_horizontalPass, horizontal);

                if (i > 0) {
                    glBindTextureUnit(0, m_PingPongMaps[!horizontal]);
                    glProgramUniform1i(m_FragmentProgID, u_brightnessMap, 0);
                }

                m_Quad->Draw(GL_TRIANGLES);
                horizontal = !horizontal;
            }

            Unbind();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        private:
            int32_t u_brightnessMap = -1;
            int32_t u_horizontalPass = -1;
            int32_t u_frameWidth = -1;
            int32_t u_frameHeight = -1;

            uint32_t m_PingPongMaps[2];
            uint32_t m_FrameBuffer[2];
            int32_t m_Height = 0;
            int32_t m_Width = 0;
            int32_t m_Scale = 5;

            quad2D m_Quad;
    };
}
