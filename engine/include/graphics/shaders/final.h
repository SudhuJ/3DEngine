#pragma once
#include "shader.h"
#include "../utilities/quad.h"

namespace flow {
    struct finalShader : public Shader {
        FLOW_INLINE finalShader(const std::string& vertFile, const std::string& fragFile) : Shader(vertFile, fragFile) {
            u_Map = glGetUniformLocation(m_FragmentProgID, "u_map");
            m_Quad = createQuad2D();
        }

        FLOW_INLINE void setSceneMap(uint32_t map) {
            glBindTextureUnit(0, map);
            glProgramUniform1i(m_FragmentProgID, u_Map, 0);
        }

        FLOW_INLINE void Show(uint32_t map) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            Bind();
            glBindTextureUnit(0, map);
            glProgramUniform1i(m_FragmentProgID, u_Map, 0);
            m_Quad->Draw(GL_TRIANGLES);
            Unbind();
        }

    private:
        int32_t u_Map = -1;
        quad2D m_Quad;
    };
}
