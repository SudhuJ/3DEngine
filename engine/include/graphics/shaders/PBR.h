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

            u_Roughness = glGetUniformLocation(m_FragmentProgID, "u_material.Roughness");
            u_Metallic  = glGetUniformLocation(m_FragmentProgID, "u_material.Metallic");
            u_Albedo    = glGetUniformLocation(m_FragmentProgID, "u_material.Albedo");
            u_Emissive  = glGetUniformLocation(m_FragmentProgID, "u_material.Emissive");
            u_Occlusion = glGetUniformLocation(m_FragmentProgID, "u_material.Occlusion");

            u_viewPos   = glGetUniformLocation(m_FragmentProgID, "u_viewPos");
            u_IrradianceMap = glGetUniformLocation(m_FragmentProgID, "u_irradMap");
            u_BRDFMap = glGetUniformLocation(m_FragmentProgID, "u_BRDFMap");
            u_PrefilteredMap = glGetUniformLocation(m_FragmentProgID, "u_PrefilteredMap");

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
        }

        FLOW_INLINE void setEnvMaps(int32_t irrad, int32_t brdf, int32_t prefil) {
            Bind();
            glBindTextureUnit(0, irrad);
            glProgramUniform1i(m_FragmentProgID, u_IrradianceMap, 0);
            glBindTextureUnit(1, brdf);
            glProgramUniform1i(m_FragmentProgID, u_BRDFMap, 1);
            glBindTextureUnit(2, prefil);
            glProgramUniform1i(m_FragmentProgID, u_PrefilteredMap, 2);
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

        FLOW_INLINE void Draw(model3D& model, PBRMaterial& material, transform3D& transform){
            glProgramUniformMatrix4fv(m_VertexProgID, u_Model, 1, GL_FALSE, glm::value_ptr(transform.Matrix()));

            glProgramUniform1f(m_FragmentProgID, u_Roughness, material.Roughness);
            glProgramUniform1f(m_FragmentProgID, u_Metallic, material.Metallic);
            glProgramUniform1f(m_FragmentProgID, u_Occlusion, material.Occlusion);

            glProgramUniform3fv(m_FragmentProgID, u_Albedo, 1, glm::value_ptr(material.Albedo));
            glProgramUniform3fv(m_FragmentProgID, u_Emissive, 1, glm::value_ptr(material.Emissive));

            int32_t unit = 3;
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

        FLOW_INLINE void setPointLight(pointLight& light, transform3D& transform, int32_t index) {
            std::string intensity = "u_pointLights[" + std::to_string(index) + "].Intensity";
            std::string radiance = "u_pointLights[" + std::to_string(index) + "].Radiance";
            std::string position = "u_pointLights[" + std::to_string(index) + "].Position";

            int32_t u_intensity = glGetUniformLocation(m_FragmentProgID, intensity.c_str());
            int32_t u_radiance = glGetUniformLocation(m_FragmentProgID, radiance.c_str());
            int32_t u_position = glGetUniformLocation(m_FragmentProgID, position.c_str());

            glProgramUniform1f(m_FragmentProgID, u_intensity, light.Intensity);
            glProgramUniform3fv(m_FragmentProgID, u_radiance, 1, glm::value_ptr(light.Radiance));
            glProgramUniform3fv(m_FragmentProgID, u_position, 1, glm::value_ptr(transform.Translate));
        }

        FLOW_INLINE void setDirectLight(directLight& light, transform3D& transform, int32_t index) {
            std::string intensity = "u_directLights[" + std::to_string(index) + "].Intensity";
            std::string radiance = "u_directLights[" + std::to_string(index) + "].Radiance";
            std::string direction = "u_directLights[" + std::to_string(index) + "].Direction";

            int32_t u_intensity = glGetUniformLocation(m_FragmentProgID, intensity.c_str());
            int32_t u_radiance = glGetUniformLocation(m_FragmentProgID, radiance.c_str());
            int32_t u_direction = glGetUniformLocation(m_FragmentProgID, direction.c_str());

            glm::vec3 forward = glm::quat(glm::radians(transform.Rotate)) * glm::vec3(0.0f, 0.0f, -1.0f);
            glProgramUniform1f(m_FragmentProgID, u_intensity, light.Intensity);
            glProgramUniform3fv(m_FragmentProgID, u_radiance, 1, glm::value_ptr(light.Radiance));
            glProgramUniform3fv(m_FragmentProgID, u_direction, 1, glm::value_ptr(forward));
        }

        FLOW_INLINE void setSpotLight(spotLight& light, transform3D& transform, int32_t index) {
            std::string intensity = "u_spotLights[" + std::to_string(index) + "].Intensity";
            std::string radiance = "u_spotLights[" + std::to_string(index) + "].Radiance";
            std::string direction = "u_spotLights[" + std::to_string(index) + "].Direction";
            std::string falloff = "u_spotLights[" + std::to_string(index) + "].Falloff";
            std::string cutoff = "u_spotLights[" + std::to_string(index) + "].Cutoff";
            std::string position = "u_spotLights[" + std::to_string(index) + "].Position";

            int32_t u_intensity = glGetUniformLocation(m_FragmentProgID, intensity.c_str());
            int32_t u_radiance = glGetUniformLocation(m_FragmentProgID, radiance.c_str());
            int32_t u_direction = glGetUniformLocation(m_FragmentProgID, direction.c_str());
            int32_t u_falloff = glGetUniformLocation(m_FragmentProgID, falloff.c_str());
            int32_t u_cutoff = glGetUniformLocation(m_FragmentProgID, cutoff.c_str());
            int32_t u_position = glGetUniformLocation(m_FragmentProgID, position.c_str());

            glProgramUniform1f(m_FragmentProgID, u_intensity, light.Intensity);
            glProgramUniform1f(m_FragmentProgID, u_falloff, glm::cos(glm::radians(light.Falloff)));
            glProgramUniform1f(m_FragmentProgID, u_cutoff, glm::cos(glm::radians(light.Cutoff)));
            glProgramUniform3fv(m_FragmentProgID, u_radiance, 1, glm::value_ptr(light.Radiance));

            glm::vec3 forward = glm::quat(glm::radians(transform.Rotate)) * glm::vec3(0.0f, 0.0f, -1.0f);
            glProgramUniform3fv(m_FragmentProgID, u_direction, 1, glm::value_ptr(forward));
            glProgramUniform3fv(m_FragmentProgID, u_position, 1, glm::value_ptr(transform.Translate));

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


        private:
            int32_t u_Model = -1;
            int32_t u_View = -1;
            int32_t u_Proj = -1;

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

            int32_t u_npointLights = -1;
            int32_t u_ndirectLights = -1;
            int32_t u_nspotLights = -1;
    };
}
