#pragma once

#include "common/core.h"
#include <stb_image.h>

namespace flow {
    struct texture2D {
        FLOW_INLINE texture2D() = default;
        FLOW_INLINE texture2D(const std::string& path) {
            Load(path);
        }

        FLOW_INLINE ~texture2D() {
            glDeleteTextures(1, &m_ID);
        }

        FLOW_INLINE bool Load(const std::string& path, bool isHDR = false, bool flipY = true) {
            stbi_set_flip_vertically_on_load(flipY);
            int32_t channels;
            void* pixels = nullptr;
            if (isHDR) {
                pixels = stbi_loadf(path.c_str(), &m_Width, &m_Height, &channels, 0);
            }
            else {
                pixels = stbi_load(path.c_str(), &m_Width, &m_Height, nullptr, 4);
            }
            if (!pixels) {
                FLOW_ERROR("Failed to load texture.");
                return false;
            }

            // mipmap levels
            int32_t maxDim = (m_Width > m_Height) ? m_Width : m_Height;
            int32_t levels = static_cast<int32_t>(std::floor(std::log2(maxDim))) + 1;

            glCreateTextures(GL_TEXTURE_2D, levels, &m_ID);

            if (isHDR) {
                glTextureStorage2D(m_ID, levels, GL_RGB16F, m_Width, m_Height);
                glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height,
                    channels == 4 ? GL_RGBA : GL_RGB, GL_FLOAT, pixels);
            }
            else {
                glTextureStorage2D(m_ID, levels, GL_RGBA8, m_Width, m_Height);
                glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            }

            glGenerateTextureMipmap(m_ID);

            glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            stbi_image_free(pixels);
            return true;
        }

        FLOW_INLINE void Use(uint32_t program_ID, uint32_t uniform, int32_t unit) {
            glBindTextureUnit(unit, m_ID);
            glProgramUniform1i(program_ID, uniform, unit);
        }

        FLOW_INLINE void Bind() {
            glBindTexture(GL_TEXTURE_2D, m_ID);
        }

        FLOW_INLINE void Unbind() {
            glBindTexture(GL_TEXTURE_2D, 0);
        }


        FLOW_INLINE operator uint32_t() const {
            return m_ID;
        }

        FLOW_INLINE int32_t getHeight() const {
            return m_Height;
        }

        FLOW_INLINE int32_t getWidth() const {
            return m_Width;
        }

        FLOW_INLINE int32_t getID() const {
            return m_ID;
        }

        private:
            int32_t m_Height = 0;
            int32_t m_Width = 0;
            uint32_t m_ID = 0u;
    };
}

using Texture = std::shared_ptr<flow::texture2D>;
