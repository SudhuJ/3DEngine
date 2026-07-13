#pragma once

#include "../buffers/mesh.h"
#include "../textures/texture.h"

namespace flow {
    struct transform3D {
        FLOW_INLINE glm::mat4 Matrix() const {
            return glm::translate(glm::mat4(1.0f), Translate) *
            glm::toMat4(glm::quat(glm::radians(Rotate))) *
            glm::scale(glm::mat4(1.0f), Scale);

        }
        glm::vec3 Translate = glm::vec3(0.0f);
        glm::vec3 Rotate = glm::vec3(0.0f);
        glm::vec3 Scale = glm::vec3(1.0f);
    };

    struct camera3D {
        FLOW_INLINE glm::mat4 Frustum(const transform3D& transform, float ratio) const {
            return Projection(ratio) * View(transform);
        }

        FLOW_INLINE glm::mat4 View(const transform3D& transform) const {
            glm::quat orientation = glm::quat(glm::radians(transform.Rotate));
            glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 up = orientation * glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 target = transform.Translate + forward;
            return glm::lookAt(transform.Translate, target, up);
        }

        FLOW_INLINE glm::mat4 Projection(float ratio) const {
            return glm::perspective(FOV, ratio, nearPlane, farPlane);
        }

        float nearPlane = 0.3f;
        float farPlane = 500.0f;
        float FOV = glm::radians(45.0f);
    };

    struct Material{
        FLOW_INLINE Material() = default;
        FLOW_INLINE Material(const Material&) = default;

        glm::vec3 Emissive = glm::vec3(0.0f);
        glm::vec3 Albedo = glm::vec3(1.0f);
        float Roughness = 0.5f;
        float Metallic = 0.2f;
        float Occlusion = 1.0f;

        Texture EmissiveMap;
        Texture AlbedoMap;
        Texture RoughnessMap;
        Texture MetallicMap;
        Texture OcclusionMap;
        Texture NormalMap;
    };

    struct pointLight {
        FLOW_INLINE pointLight(const pointLight&) = default;
        FLOW_INLINE pointLight() = default;

        glm::vec3 Radiance = glm::vec3(1.0f);
        float Intensity = 1.0f;
    };

    struct directLight {
        FLOW_INLINE directLight(const directLight&) = default;
        FLOW_INLINE directLight() = default;

        glm::vec3 Radiance = glm::vec3(1.0f);
        float Intensity = 1.0f;
    };

    struct  spotLight {
        FLOW_INLINE spotLight(const spotLight&) = default;
        FLOW_INLINE spotLight() = default;

        glm::vec3 Radiance = glm::vec3(1.0f);
        float Intensity = 1.0f;
        float Falloff = 60.0f;
        float Cutoff = 20.0f;
    };

    struct Skybox {
        FLOW_INLINE Skybox() = default;
        FLOW_INLINE Skybox(const Skybox&) = default;

        uint32_t m_CubeMap = 0;
        uint32_t IrradianceMap = 0;
        uint32_t PrefilteredMap = 0;
        uint32_t BRDFMap = 0;
    };

}
