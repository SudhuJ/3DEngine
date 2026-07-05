#pragma once

#include "vertex.h"

namespace flow {
    template <typename Vertex>
    struct Mesh {
        FLOW_INLINE Mesh(const meshData<Vertex>& data) {
            if (data.vertices.empty()) {
                FLOW_ERROR("Mesh Data Empty.");
                return;
            }
            m_VertexSize = data.vertices.size();
            m_IndexSize = data.indices.size();

            glCreateVertexArrays(1, &m_BufferID);

            glCreateBuffers(1, &m_VBOID);

            glNamedBufferStorage(m_VBOID, m_VertexSize * sizeof(Vertex), data.vertices.data(), 0);
            glVertexArrayVertexBuffer(m_BufferID, 0, m_VBOID, 0, sizeof(Vertex));

            if (m_IndexSize != 0u) {
                glCreateBuffers(1, &m_EBOID);
                glNamedBufferStorage(m_EBOID, m_IndexSize * sizeof(uint32_t), data.indices.data(), 0);
                glVertexArrayElementBuffer(m_BufferID, m_EBOID);
            }

            // vertex type handle
            if (TypeID<Vertex>() == TypeID<shadedVertex>()) {
                setAttribute(0, 3, offsetof(shadedVertex, Position));
                setAttribute(1, 3, offsetof(shadedVertex, Normal));
                setAttribute(2, 2, offsetof(shadedVertex, UVs));
                setAttribute(3, 3, offsetof(shadedVertex, Tangent));
                setAttribute(4, 3, offsetof(shadedVertex, Bitangent));
            }

            else if (TypeID<Vertex>() == TypeID<flatVertex>()) {
                setAttribute(0, 3, offsetof(flatVertex, Position));
                setAttribute(1, 4, offsetof(flatVertex, Color));
            }

            else if (TypeID<Vertex>() == TypeID<quadVertex>()) {
                setAttribute(0, 4, offsetof(quadVertex, data));
            }

            else if (TypeID<Vertex>() == TypeID<skyboxVertex>()) {
                setAttribute(0, 3, offsetof(skyboxVertex, Position));
            }

            else {
                FLOW_ERROR(false && "Invalid Vertex Type.");
            }
        }

        FLOW_INLINE void Draw(uint32_t mode) {
            glBindVertexArray(m_BufferID);
            if (m_IndexSize != 0u) {
                glDrawElements(mode, m_IndexSize, GL_UNSIGNED_INT, nullptr);
            }
            else {
                glDrawArrays(mode, 0, m_VertexSize);
            }
            glBindVertexArray(0);
        }

        FLOW_INLINE ~Mesh() {
            glDeleteVertexArrays(1, &m_BufferID);
            glDeleteBuffers(1, &m_VBOID);
            if (m_EBOID != 0u) {
                glDeleteBuffers(1, &m_EBOID);
            }
        }

        private:

            FLOW_INLINE void setAttribute(uint32_t index, int32_t elementCount, size_t offset) {
                glEnableVertexArrayAttrib(m_BufferID, index);
                glVertexArrayAttribFormat(m_BufferID, index, elementCount, GL_FLOAT, GL_FALSE, static_cast<uint32_t>(offset));
                glVertexArrayAttribBinding(m_BufferID, index, 0);
            }

            uint32_t m_VertexSize = 0u;
            uint32_t m_IndexSize = 0u;
            uint32_t m_BufferID = 0u;
            uint32_t m_VBOID = 0u;
            uint32_t m_EBOID = 0u;
    };
    using shadedMesh = Mesh<shadedVertex>;
    using mesh3D = std::shared_ptr<shadedMesh>;
}
