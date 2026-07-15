#pragma once
#include "shader.h"
#include "../utilities/skybox.h"
#include "../utilities/data.h"

namespace flow {
    struct SkyboxShader : Shader {
        FLOW_INLINE SkyboxShader(const std::string& vertFile, const std::string& fragFile) :Shader(vertFile, fragFile){
            u_Model = glGetUniformLocation(m_VertexProgID, "u_model");
            u_View = glGetUniformLocation(m_VertexProgID, "u_view");
            u_Proj = glGetUniformLocation(m_VertexProgID, "u_proj");
            u_Map = glGetUniformLocation(m_FragmentProgID, "u_map");
        }

        FLOW_INLINE void setCamera(camera3D& camera, transform3D& transform, float ratio) {
            glProgramUniformMatrix4fv(m_VertexProgID, u_Proj, 1, GL_FALSE, glm::value_ptr(camera.Projection(ratio)));
            glProgramUniformMatrix4fv(m_VertexProgID, u_View, 1, GL_FALSE, glm::value_ptr(camera.View(transform)));
        }

        FLOW_INLINE void Draw(skyboxMesh& mesh, uint32_t cubeMap, transform3D& transform) {
            Bind();
            glProgramUniformMatrix4fv(m_VertexProgID, u_Model, 1, GL_FALSE, glm::value_ptr(transform.Matrix()));
            glBindTextureUnit(0, cubeMap);
            glProgramUniform1i(m_FragmentProgID, u_Map, 0);
            renderSkyboxMesh(mesh);
            Unbind();
        }

        private:
            int32_t u_Model = 0;
            int32_t u_View = 0;
            int32_t u_Proj = 0;
            int32_t u_Map = 0;
    };
}
