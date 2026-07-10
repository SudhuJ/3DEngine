#pragma once
#include "shader.h"
#include "../utilities/data.h"
#include "../models/model.h"

namespace flow {
    struct PBRShader : public Shader {
        FLOW_INLINE PBRShader(const std::string& vertFile, const std::string& fragFile) : Shader(vertFile, fragFile) {
            u_Model = glGetUniformLocation(m_VertexProgID, "u_model");
            u_View = glGetUniformLocation(m_VertexProgID, "u_view");
            u_Proj = glGetUniformLocation(m_VertexProgID, "u_proj");
            u_hasJoints = glGetUniformLocation(m_VertexProgID, "u_hasJoints");

            u_Roughness = glGetUniformLocation(m_FragmentProgID, "u_material.Roughness");
            u_Metallic  = glGetUniformLocation(m_FragmentProgID, "u_material.Metallic");
            u_Albedo    = glGetUniformLocation(m_FragmentProgID, "u_material.Albedo");
            u_Emissive  = glGetUniformLocation(m_FragmentProgID, "u_material.Emissive");
            u_Occlusion = glGetUniformLocation(m_FragmentProgID, "u_material.Occlusion");

            u_viewPos   = glGetUniformLocation(m_FragmentProgID, "u_viewPos");
            u_IrradianceMap = glGetUniformLocation(m_FragmentProgID, "u_IrradMap");
            u_BRDFMap = glGetUniformLocation(m_FragmentProgID, "u_BRDFMap");
            u_PrefilteredMap = glGetUniformLocation(m_FragmentProgID, "u_PrefilteredMap");
            u_DepthMap = glGetUniformLocation(m_FragmentProgID, "u_DepthMap");
            u_LightSpace = glGetUniformLocation(m_FragmentProgID, "u_LightSpace");

            u_npointLights = glGetUniformLocation(m_FragmentProgID, "u_npointLights");
            u_ndirectLights = glGetUniformLocation(m_FragmentProgID, "u_ndirectLights");
            u_nspotLights = glGetUniformLocation(m_FragmentProgID, "u_nspotLights");

            u_RoughnessMap = glGetUniformLocation(m_FragmentProgID, "u_material.RoughnessMap");
            u_OcclusionMap = glGetUniformLocation(m_FragmentProgID, "u_material.OcclusionMap");
            u_EmissiveMap = glGetUniformLocation(m_FragmentProgID, "u_material.EmissiveMap");
            u_MetallicMap = glGetUniformLocation(m_FragmentProgID, "u_material.MetallicMap");
            u_AlbedoMap = glGetUniformLocation(m_FragmentProgID, "u_material.AlbedoMap");
            u_NormalMap = glGetUniformLocation(m_FragmentProgID, "u_material.NormalMap");

            u_useRoughnessMap = glGetUniformLocation(m_FragmentProgID, "u_material.useRoughnessMap");
            u_useOcclusionMap = glGetUniformLocation(m_FragmentProgID, "u_material.useOcclusionMap");
            u_useEmissiveMap = glGetUniformLocation(m_FragmentProgID, "u_material.useEmissiveMap");
            u_useMetallicMap = glGetUniformLocation(m_FragmentProgID, "u_material.useMetallicMap");
            u_useAlbedoMap = glGetUniformLocation(m_FragmentProgID, "u_material.useAlbedoMap");
            u_useNormalMap = glGetUniformLocation(m_FragmentProgID, "u_material.useNormalMap");

            for (int32_t i = 0; i < kMaxLights; i++) {
                std::string idx = std::to_string(i);
                u_pointLight_Intensity[i] = glGetUniformLocation(m_FragmentProgID, ("u_pointLights[" + idx + "].Intensity").c_str());
                u_pointLight_Radiance[i] = glGetUniformLocation(m_FragmentProgID, ("u_pointLights[" + idx + "].Radiance").c_str());
                u_pointLight_Position[i] = glGetUniformLocation(m_FragmentProgID, ("u_pointLights[" + idx + "].Position").c_str());

                u_directLight_Intensity[i] = glGetUniformLocation(m_FragmentProgID, ("u_directLights[" + idx + "].Intensity").c_str());
                u_directLight_Radiance[i] = glGetUniformLocation(m_FragmentProgID, ("u_directLights[" + idx + "].Radiance").c_str());
                u_directLight_Direction[i] = glGetUniformLocation(m_FragmentProgID, ("u_directLights[" + idx + "].Direction").c_str());

                u_spotLight_Intensity[i] = glGetUniformLocation(m_FragmentProgID, ("u_spotLights[" + idx + "].Intensity").c_str());
                u_spotLight_Radiance[i] = glGetUniformLocation(m_FragmentProgID, ("u_spotLights[" + idx + "].Radiance").c_str());
                u_spotLight_Direction[i] = glGetUniformLocation(m_FragmentProgID, ("u_spotLights[" + idx + "].Direction").c_str());
                u_spotLight_Falloff[i] = glGetUniformLocation(m_FragmentProgID, ("u_spotLights[" + idx + "].Falloff").c_str());
                u_spotLight_Cutoff[i] = glGetUniformLocation(m_FragmentProgID, ("u_spotLights[" + idx + "].Cutoff").c_str());
                u_spotLight_Position[i] = glGetUniformLocation(m_FragmentProgID, ("u_spotLights[" + idx + "].Position").c_str());
            }
        }

        FLOW_INLINE void setEnvMaps(int32_t irrad, int32_t brdf, int32_t prefil, int32_t depth) {
            Bind();
            glBindTextureUnit(0, irrad);
            glProgramUniform1i(m_FragmentProgID, u_IrradianceMap, 0);
            glBindTextureUnit(1, brdf);
            glProgramUniform1i(m_FragmentProgID, u_BRDFMap, 1);
            glBindTextureUnit(2, prefil);
            glProgramUniform1i(m_FragmentProgID, u_PrefilteredMap, 2);
            glBindTextureUnit(3, depth);
            glProgramUniform1i(m_FragmentProgID, u_DepthMap, 3);
        }

        FLOW_INLINE void setCamera(camera3D& camera, transform3D& transform, float ratio){
            glProgramUniformMatrix4fv(m_VertexProgID, u_Proj, 1, GL_FALSE, glm::value_ptr(camera.Projection(ratio)));
            glProgramUniformMatrix4fv(m_VertexProgID, u_View, 1, GL_FALSE, glm::value_ptr(camera.View(transform)));
            glProgramUniform3fv(m_FragmentProgID, u_viewPos, 1, glm::value_ptr(transform.Translate));
        }

        FLOW_INLINE void Draw(mesh3D& mesh, transform3D& transform){
            glProgramUniformMatrix4fv(m_VertexProgID, u_Model, 1, GL_FALSE, glm::value_ptr(transform.Matrix()));
            mesh->Draw(GL_TRIANGLES);
        }

        FLOW_INLINE void Draw(model3D& model, Material& material, transform3D& transform){
            glProgramUniformMatrix4fv(m_VertexProgID, u_Model, 1, GL_FALSE, glm::value_ptr(transform.Matrix()));
            glProgramUniform1i(m_VertexProgID, u_hasJoints, model->HasJoint());
            glProgramUniform1f(m_FragmentProgID, u_Roughness, material.Roughness);
            glProgramUniform1f(m_FragmentProgID, u_Metallic, material.Metallic);
            glProgramUniform1f(m_FragmentProgID, u_Occlusion, material.Occlusion);

            glProgramUniform3fv(m_FragmentProgID, u_Albedo, 1, glm::value_ptr(material.Albedo));
            glProgramUniform3fv(m_FragmentProgID, u_Emissive, 1, glm::value_ptr(material.Emissive));

            int32_t unit = 4;
            bool useMap = false;

            useMap = material.AlbedoMap != nullptr;
            glProgramUniform1i(m_FragmentProgID, u_useAlbedoMap, useMap);
            if(material.AlbedoMap) {
                material.AlbedoMap->Use(m_FragmentProgID, u_AlbedoMap, unit++);
            }

            useMap = material.NormalMap != nullptr;
            glProgramUniform1i(m_FragmentProgID, u_useNormalMap, useMap);
            if(material.NormalMap) {
                material.NormalMap->Use(m_FragmentProgID, u_NormalMap, unit++);
            }

            useMap = material.MetallicMap != nullptr;
            glProgramUniform1i(m_FragmentProgID, u_useMetallicMap, useMap);
            if(material.MetallicMap) {
                material.MetallicMap->Use(m_FragmentProgID, u_MetallicMap, unit++);
            }

            useMap = material.EmissiveMap != nullptr;
            glProgramUniform1i(m_FragmentProgID, u_useEmissiveMap, useMap);
            if(material.EmissiveMap) {
                material.EmissiveMap->Use(m_FragmentProgID, u_EmissiveMap, unit++);
            }

            useMap = material.OcclusionMap != nullptr;
            glProgramUniform1i(m_FragmentProgID, u_useOcclusionMap, useMap);
            if(material.OcclusionMap) {
                material.OcclusionMap->Use(m_FragmentProgID, u_OcclusionMap, unit++);
            }

            useMap = material.RoughnessMap != nullptr;
            glProgramUniform1i(m_FragmentProgID, u_useRoughnessMap, useMap);
            if(material.RoughnessMap) {
                material.RoughnessMap->Use(m_FragmentProgID, u_RoughnessMap, unit++);
            }

            model->Draw(GL_TRIANGLES);
        }

        FLOW_INLINE void setJoints(std::vector<glm::mat4>& transforms) {
            for (size_t i = 0; i < transforms.size(); ++i) {
                std::string uniform = "u_joints[" + std::to_string(i) + "]";
                GLint loc = glGetUniformLocation(m_VertexProgID, uniform.c_str());
                if (loc != -1) {
                    glProgramUniformMatrix4fv(m_VertexProgID, loc, 1, GL_FALSE, glm::value_ptr(transforms[i]));
                }
                else {
                    FLOW_ERROR("u_joints Location Not Set.");
                }
            }
        }

        FLOW_INLINE void setPointLight(pointLight& light, transform3D& transform, int32_t index) {
            glProgramUniform1f(m_FragmentProgID, u_pointLight_Intensity[index], light.Intensity);
            glProgramUniform3fv(m_FragmentProgID, u_pointLight_Radiance[index], 1, glm::value_ptr(light.Radiance));
            glProgramUniform3fv(m_FragmentProgID, u_pointLight_Position[index], 1, glm::value_ptr(transform.Translate));
        }

        FLOW_INLINE void setDirectLight(directLight& light, transform3D& transform, int32_t index) {
            glProgramUniform1f(m_FragmentProgID, u_directLight_Intensity[index], light.Intensity);
            glProgramUniform3fv(m_FragmentProgID, u_directLight_Radiance[index], 1, glm::value_ptr(light.Radiance));
            glProgramUniform3fv(m_FragmentProgID, u_directLight_Direction[index], 1, glm::value_ptr(transform.Rotate));
        }

        FLOW_INLINE void setSpotLight(spotLight& light, transform3D& transform, int32_t index) {
            glProgramUniform1f(m_FragmentProgID, u_spotLight_Intensity[index], light.Intensity);
            glProgramUniform1f(m_FragmentProgID, u_spotLight_Falloff[index], glm::cos(glm::radians(light.Falloff)));
            glProgramUniform1f(m_FragmentProgID, u_spotLight_Cutoff[index], glm::cos(glm::radians(light.Cutoff)));
            glProgramUniform3fv(m_FragmentProgID, u_spotLight_Radiance[index], 1, glm::value_ptr(light.Radiance));
            glProgramUniform3fv(m_FragmentProgID, u_spotLight_Direction[index], 1, glm::value_ptr(transform.Rotate));
            glProgramUniform3fv(m_FragmentProgID, u_spotLight_Position[index], 1, glm::value_ptr(transform.Translate));
        }

        FLOW_INLINE void setPointLightCount(int32_t count) {
            glProgramUniform1i(m_FragmentProgID, u_npointLights, count);
        }

        FLOW_INLINE void setDirectLightCount(int32_t count) {
            glProgramUniform1i(m_FragmentProgID, u_ndirectLights, count);
        }

        FLOW_INLINE void setSpotLightCount(int32_t count) {
            glProgramUniform1i(m_FragmentProgID, u_nspotLights, count);
        }

        FLOW_INLINE void setLightSpaceMatrix(glm::mat4& lightSpace) {
            glProgramUniformMatrix4fv(m_FragmentProgID, u_LightSpace, 1, GL_FALSE, glm::value_ptr(lightSpace));
        }

        private:
            static constexpr int32_t kMaxLights = 10;

            int32_t u_Model = -1;
            int32_t u_View = -1;
            int32_t u_Proj = -1;
            int32_t u_hasJoints = -1;

            int32_t u_Roughness = -1;
            int32_t u_Metallic = -1;
            int32_t u_Albedo = -1;
            int32_t u_Emissive = -1;
            int32_t u_Occlusion = -1;

            int32_t u_useRoughnessMap = -1;
            int32_t u_useOcclusionMap = -1;
            int32_t u_useEmissiveMap = -1;
            int32_t u_useMetallicMap = -1;
            int32_t u_useAlbedoMap = -1;
            int32_t u_useNormalMap = -1;

            int32_t u_RoughnessMap = -1;
            int32_t u_OcclusionMap = -1;
            int32_t u_EmissiveMap = -1;
            int32_t u_MetallicMap = -1;
            int32_t u_AlbedoMap = -1;
            int32_t u_NormalMap = -1;

            int32_t u_viewPos = -1;
            int32_t u_IrradianceMap = -1;
            int32_t u_BRDFMap = -1;
            int32_t u_PrefilteredMap = -1;
            int32_t u_LightSpace = -1;
            int32_t u_DepthMap = -1;

            int32_t u_npointLights = -1;
            int32_t u_ndirectLights = -1;
            int32_t u_nspotLights = -1;

            int32_t u_pointLight_Intensity[kMaxLights]{};
            int32_t u_pointLight_Radiance[kMaxLights]{};
            int32_t u_pointLight_Position[kMaxLights]{};

            int32_t u_directLight_Intensity[kMaxLights]{};
            int32_t u_directLight_Radiance[kMaxLights]{};
            int32_t u_directLight_Direction[kMaxLights]{};

            int32_t u_spotLight_Intensity[kMaxLights]{};
            int32_t u_spotLight_Radiance[kMaxLights]{};
            int32_t u_spotLight_Direction[kMaxLights]{};
            int32_t u_spotLight_Falloff[kMaxLights]{};
            int32_t u_spotLight_Cutoff[kMaxLights]{};
            int32_t u_spotLight_Position[kMaxLights]{};
    };
}
