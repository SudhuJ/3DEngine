#pragma once
#include "../buffers/mesh.h"

namespace flow {
    using quad2D = std::unique_ptr<Mesh<quadVertex>>;
    using quad3D = std::unique_ptr<Mesh<shadedVertex>>;

    FLOW_INLINE quad2D createQuad2D() {
        meshData<quadVertex> data;
        data.vertices = {
            quadVertex({-1.0f, -1.0f, 0.0f, 0.0f}),
            quadVertex({-1.0f,  1.0f, 0.0f, 1.0f}),
            quadVertex({ 1.0f,  1.0f, 1.0f, 1.0f}),
            quadVertex({ 1.0f, -1.0f, 1.0f, 0.0f}),
        };
        data.indices = {
            0, 1, 2,
            0, 2, 3,
        };

        return std::make_unique<Mesh<quadVertex>>(std::move(data));
    }


    FLOW_INLINE quad3D createQuad3D() {
        meshData<shadedVertex> data;

        shadedVertex v0, v1, v2, v3;

        v0.Position = glm::vec3(-1.0f, -1.0f, 0.0f);
        v1.Position = glm::vec3( 1.0f, -1.0f, 0.0f);
        v2.Position = glm::vec3( 1.0f,  1.0f, 0.0f);
        v3.Position = glm::vec3(-1.0f,  1.0f, 0.0f);

        v0.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
        v1.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
        v2.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
        v3.Normal = glm::vec3(0.0f, 0.0f, 1.0f);

        v0.UVs = glm::vec2(0.0f, 0.0f);
        v1.UVs = glm::vec2(1.0f, 0.0f);
        v2.UVs = glm::vec2(1.0f, 1.0f);
        v3.UVs = glm::vec2(0.0f, 1.0f);

        data.vertices = {v0, v1, v2, v3};
        data.indices = {
            0, 1, 2,
            0, 2, 3,
        };

        return std::make_unique<Mesh<shadedVertex>>(std::move(data));
    }
}
