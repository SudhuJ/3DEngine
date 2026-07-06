#pragma once
#include "shader.h"
#include "../utilities/quad.h"

namespace flow {
    struct finalShader : public Shader {
        FLOW_INLINE finalShader(const std::string& vertFile, const std::string& fragFile) : Shader(vertFile, fragFile) {
            u_Map = glGetUniformLocation(m_FragmentProgID, "u_map");
            u_Bloom = glGetUniformLocation(m_FragmentProgID, "u_bloom");
            m_Quad = createQuad2D();
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

    private:
        int32_t u_Map = -1;
        int32_t u_Bloom = -1;
        quad2D m_Quad;
    };
}
