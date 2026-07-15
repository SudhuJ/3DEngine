#pragma once
#include "common/core.h"

namespace flow {
    struct frameBuffer {
        FLOW_INLINE frameBuffer(uint32_t width, uint32_t height) :
            m_Width(width), m_Height(height) {
            glCreateFramebuffers(1, &m_BufferID);

            createColorAttachment();
            createBrightnessAttachment();
            createRenderAttachment();

            uint32_t attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
            glNamedFramebufferDrawBuffers(m_BufferID, 2, attachments);

            if (glCheckNamedFramebufferStatus(m_BufferID, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                FLOW_ERROR("glCheckNamedFramebufferStatus() Failed in Constructor.");
            }
        }

        FLOW_INLINE ~frameBuffer() {
            glDeleteTextures(1, &m_Color);
            glDeleteTextures(1, &m_Brightness);
            glDeleteRenderbuffers(1, &m_Render);
            glDeleteFramebuffers(1, &m_BufferID);
        }

        FLOW_INLINE uint32_t getWidth() const {
            return m_Width;
        }

        FLOW_INLINE uint32_t getHeight() const {
            return m_Height;
        }

        FLOW_INLINE float Ratio() {
            return (float)m_Width / (float)m_Height;
        }

        FLOW_INLINE void Resize(int32_t width, int32_t height) {

            if (width < 1 || height < 1) return;

            m_Width = width;
            m_Height = height;
            glDeleteTextures(1, &m_Color);
            glDeleteTextures(1, &m_Brightness);
            glDeleteRenderbuffers(1, &m_Render);

            createColorAttachment();
            createBrightnessAttachment();
            createRenderAttachment();

            uint32_t attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
            glNamedFramebufferDrawBuffers(m_BufferID, 2, attachments);

            if (glCheckNamedFramebufferStatus(m_BufferID, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                FLOW_ERROR("glCheckNamedFramebufferStatus() Failed to Initializer.");
            }
        }

        FLOW_INLINE uint32_t getTexture() {
            return m_Color;
        }

        FLOW_INLINE uint32_t getBrightnessTexture() {
            return m_Brightness;
        }

        FLOW_INLINE void Begin() {
            glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
            glViewport(0, 0, m_Width, m_Height);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
        }

        FLOW_INLINE void End() {
            glDisable(GL_DEPTH_TEST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        private:

            FLOW_INLINE void createColorAttachment() {
                glCreateTextures(GL_TEXTURE_2D, 1, &m_Color);
                glTextureParameteri(m_Color, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTextureParameteri(m_Color, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTextureParameteri(m_Color, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTextureParameteri(m_Color, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTextureStorage2D(m_Color, 1, GL_RGB16F, m_Width, m_Height);
                glNamedFramebufferTexture(m_BufferID, GL_COLOR_ATTACHMENT0, m_Color, 0);
            }

            FLOW_INLINE void createBrightnessAttachment() {
                glCreateTextures(GL_TEXTURE_2D, 1, &m_Brightness);
                glTextureParameteri(m_Brightness, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTextureParameteri(m_Brightness, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTextureParameteri(m_Brightness, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTextureParameteri(m_Brightness, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTextureStorage2D(m_Brightness, 1, GL_RGB16F, m_Width, m_Height);
                glNamedFramebufferTexture(m_BufferID, GL_COLOR_ATTACHMENT1, m_Brightness, 0);
            }

            FLOW_INLINE void createRenderAttachment() {
                glCreateRenderbuffers(1, &m_Render);
                glNamedRenderbufferStorage(m_Render, GL_DEPTH_COMPONENT24, m_Width, m_Height);
                glNamedFramebufferRenderbuffer(m_BufferID, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_Render);
            }

            uint32_t m_Width = 0u;
            uint32_t m_Height = 0u;
            uint32_t m_Color = 0u;
            uint32_t m_Brightness = 0u;
            uint32_t m_Render = 0u;
            uint32_t m_BufferID = 0u;
    };
}
