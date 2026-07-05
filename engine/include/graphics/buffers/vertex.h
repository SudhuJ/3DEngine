#pragma once

#include <common/core.h>
#include <glm/glm.hpp>

namespace flow {
    struct quadVertex {
        float data[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    };

    struct flatVertex {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec4 Color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    };

    struct shadedVertex {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec3 Normal = glm::vec3(0.0f);
        glm::vec2 UVs = glm::vec2(0.0f);

        glm::vec3 Tangent = glm::vec3(0.0f);
        glm::vec3 Bitangent = glm::vec3(0.0f);
    };

    template <typename Vertex>
    struct meshData {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    struct skyboxVertex {
        glm::vec3 Position = glm::vec3(0.0f);
    };
}
