#pragma once
#include "common/core.h"

namespace flow {
    struct Shader {
        FLOW_INLINE Shader(const std::string& vertFile, const std::string& fragFile) {
            m_ShaderID = Load(vertFile, fragFile);
        }

        FLOW_INLINE virtual ~Shader() {
            glDeleteProgramPipelines(1, &m_ShaderID);
            glDeleteProgram(m_VertexProgID);
            glDeleteProgram(m_FragmentProgID);
        }

        FLOW_INLINE void Unbind() {
            glBindProgramPipeline(0);
        }

        FLOW_INLINE void Bind() {
            glBindProgramPipeline(m_ShaderID);
        }

        private:
            FLOW_INLINE uint32_t Build(const char* src, uint32_t type) {
                uint32_t programID = glCreateShaderProgramv(type, 1, &src);
                int32_t status = 0;
                glGetProgramiv(programID, GL_LINK_STATUS, &status);
                if (status != GL_TRUE) {
                    char error[512];
                    glGetProgramInfoLog(programID, 512, nullptr, error);
                    glDeleteProgram(programID);
                    throw std::runtime_error(error);
                }
                return programID;
            }

            FLOW_INLINE std::string ReadFile(const std::string& path) {
                std::ifstream fs(path);
                if (!fs.is_open()) {
                    FLOW_ERROR("ReadFile('{}') Failed: Unable to open file stream.", path);
                    return {};
                }
                std::stringstream ss;
                ss << fs.rdbuf();
                return ss.str();
            }

            FLOW_INLINE uint32_t Load(const std::string& vertFile, const std::string& fragFile) {
                std::string vtxSrc = ReadFile(vertFile);
                std::string fragSrc = ReadFile(fragFile);

                if (vtxSrc.empty() || fragSrc.empty()) {
                    FLOW_ERROR("Shader loading aborted for '{}/{}': One or more stages failed to load.", vertFile, fragFile);
                    return 0u;
                }

                try {
                    m_VertexProgID = Build(vtxSrc.c_str(), GL_VERTEX_SHADER);
                    m_FragmentProgID = Build(fragSrc.c_str(), GL_FRAGMENT_SHADER);

                    glCreateProgramPipelines(1, &m_ShaderID);
                    glUseProgramStages(m_ShaderID, GL_VERTEX_SHADER_BIT, m_VertexProgID);
                    glUseProgramStages(m_ShaderID, GL_FRAGMENT_SHADER_BIT, m_FragmentProgID);

                    return m_ShaderID;
                }
                catch (const std::exception& e) {
                    FLOW_ERROR("Load('{}/{}') Pipeline Link Failure: {}", vertFile, fragFile, e.what());
                }
                return 0u;
            }

        protected:
            uint32_t m_ShaderID = 0u;
            uint32_t m_VertexProgID = 0u;
            uint32_t m_FragmentProgID = 0u;
    };
}
