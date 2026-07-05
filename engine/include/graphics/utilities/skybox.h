#pragma once

#include "../buffers/mesh.h"

namespace flow {
    using skyboxMesh = std::unique_ptr<Mesh<skyboxVertex>>;

    FLOW_INLINE void renderSkyboxMesh(skyboxMesh& mesh) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        mesh->Draw(GL_TRIANGLES);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_CULL_FACE);
    }

    FLOW_INLINE skyboxMesh createSkyboxMesh() {
        std::vector<glm::vec3> positions = {
            // Back Face (6 vertices)
            {-1.0f,  1.0f, -1.0f},
            {-1.0f, -1.0f, -1.0f},
            { 1.0f, -1.0f, -1.0f},
            { 1.0f, -1.0f, -1.0f},
            { 1.0f,  1.0f, -1.0f},
            {-1.0f,  1.0f, -1.0f},

            // Left Face (6 vertices)
            {-1.0f, -1.0f,  1.0f},
            {-1.0f, -1.0f, -1.0f},
            {-1.0f,  1.0f, -1.0f},
            {-1.0f,  1.0f, -1.0f},
            {-1.0f,  1.0f,  1.0f},
            {-1.0f, -1.0f,  1.0f},

            // Right Face (6 vertices)
            { 1.0f, -1.0f, -1.0f},
            { 1.0f, -1.0f,  1.0f},
            { 1.0f,  1.0f,  1.0f},
            { 1.0f,  1.0f,  1.0f},
            { 1.0f,  1.0f, -1.0f},
            { 1.0f, -1.0f, -1.0f},

            // Front Face (6 vertices)
            {-1.0f, -1.0f,  1.0f},
            {-1.0f,  1.0f,  1.0f},
            { 1.0f,  1.0f,  1.0f},
            { 1.0f,  1.0f,  1.0f},
            { 1.0f, -1.0f,  1.0f},
            {-1.0f, -1.0f,  1.0f},

            // Top Face (6 vertices)
            {-1.0f,  1.0f, -1.0f},
            { 1.0f,  1.0f, -1.0f},
            { 1.0f,  1.0f,  1.0f},
            { 1.0f,  1.0f,  1.0f},
            {-1.0f,  1.0f,  1.0f},
            {-1.0f,  1.0f, -1.0f},

            // Bottom Face (6 vertices)
            {-1.0f, -1.0f, -1.0f},
            {-1.0f, -1.0f,  1.0f},
            { 1.0f, -1.0f, -1.0f},
            { 1.0f, -1.0f, -1.0f},
            {-1.0f, -1.0f,  1.0f},
            { 1.0f, -1.0f,  1.0f}
        };

        meshData<skyboxVertex> data;
        for (uint32_t i=0; i<positions.size(); i++) {
            data.vertices.push_back({positions[i]});
        }

        return std::make_unique<Mesh<skyboxVertex>>(data);
    }




}
