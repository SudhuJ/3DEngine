#pragma once

#include "common/core.h"
#include <stb_image.h>

namespace flow {
    struct texture2D {
        FLOW_INLINE texture2D() = default;
        FLOW_INLINE texture2D(const std::string& path) {
            Load(path);
        }
        FLOW_INLINE texture2D(const std::string& path, bool isHDR, bool flipY) {
            Load(path, isHDR, flipY);
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
                FLOW_ERROR("Failed to load texture: {}.", path);
                return false;
            }

            // mipmap levels
            int32_t maxDim = (m_Width > m_Height) ? m_Width : m_Height;
            int32_t levels = static_cast<int32_t>(std::floor(std::log2(maxDim))) + 1;

            glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

            if (isHDR) {
                GLenum hdrFormat = GL_RGB;
                GLenum hdrInternal = GL_RGB16F;
                if (channels == 4) { hdrFormat = GL_RGBA; hdrInternal = GL_RGBA16F; }
                else if (channels == 2) { hdrFormat = GL_RG; hdrInternal = GL_RG16F; }
                else if (channels == 1) { hdrFormat = GL_RED; hdrInternal = GL_R16F; }
                glTextureStorage2D(m_ID, levels, hdrInternal, m_Width, m_Height);
                glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height,
                    hdrFormat, GL_FLOAT, pixels);
            }
            else {
                glTextureStorage2D(m_ID, levels, GL_RGBA8, m_Width, m_Height);
                glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            }

            glGenerateTextureMipmap(m_ID);

            glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, isHDR ? GL_REPEAT : GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            stbi_image_free(pixels);
            return true;
        }

        FLOW_INLINE static std::shared_ptr<texture2D> PackORM(const std::string& ao, const std::string& rough, const std::string& metal) {
            struct Stb {
                uint8_t* p = nullptr;
                int32_t w = 0;
                int32_t h = 0;
            };
            Stb src[3];
            int32_t width = 0;
            int32_t height = 0;

            const std::string* paths[3] = { &ao, &rough, &metal};

            for (int32_t i=0; i<3; i++) {
                if (paths[i]->empty()) continue;
                int32_t ch = 0;
                stbi_set_flip_vertically_on_load(true);
                src[i].p = stbi_load(paths[i]->c_str(), &src[i].w, &src[i].h, &ch, 4);
                if (!src[i].p) {
                    FLOW_ERROR("PackORM: Failed to load map: {}", *paths[i]);
                    continue;
                }
                if (src[i].w > width)  width = src[i].w;
                if (src[i].h > height) height = src[i].h;
            }
            if (width <= 0 || height <= 0) return nullptr;

            // R=occlusion, G=roughness, B=metalness; missing channels stay 255 (1.0)
            std::vector<uint8_t> packed(static_cast<size_t>(width) * height * 3, 255);
            for (int32_t i = 0; i < 3; ++i) {
                if (!src[i].p) continue;
                for (int32_t y = 0; y < src[i].h; ++y) {
                    for (int32_t x = 0; x < src[i].w; ++x) {
                        packed[(y * width + x) * 3 + i] = src[i].p[(y * src[i].w + x) * 4];
                    }
                }
                stbi_image_free(src[i].p);
            }
            auto tex = std::make_shared<texture2D>();
            int32_t maxDim = (width > height) ? width : height;
            int32_t levels = static_cast<int32_t>(std::floor(std::log2(maxDim))) + 1;
            glCreateTextures(GL_TEXTURE_2D, 1, &tex->m_ID);
            glTextureStorage2D(tex->m_ID, levels, GL_RGB8, width, height);
            glTextureSubImage2D(tex->m_ID, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, packed.data());
            glGenerateTextureMipmap(tex->m_ID);
            glTextureParameteri(tex->m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(tex->m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(tex->m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(tex->m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
            tex->m_Width = width;
            tex->m_Height = height;
            return tex;
        }

        FLOW_INLINE void Use(uint32_t program_ID, int32_t uniform, int32_t unit) {
            glBindTextureUnit(unit, m_ID);
            glProgramUniform1i(program_ID, uniform, unit);
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

        FLOW_INLINE uint32_t getID() const {
            return m_ID;
        }

        private:
            int32_t m_Height = 0;
            int32_t m_Width = 0;
            uint32_t m_ID = 0u;
    };
}

using Texture = std::shared_ptr<flow::texture2D>;
