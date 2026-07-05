#pragma once
#include "../buffers/mesh.h"
#include <assimp/quaternion.h>
#include <assimp/color4.h>
#include <assimp/matrix4x4.h>

namespace flow {
    FLOW_INLINE static glm::vec3 assimptoVec3(const aiVector3D& v) {
        return glm::vec3(v.x, v.y, v.z);
    }

    FLOW_INLINE static glm::vec4 assimptoVec4(const aiColor4D& c) {
        return glm::vec4(c.r, c.g, c.b, c.a);
    }

    FLOW_INLINE static glm::quat assimptoQuat(const aiQuaternion& q) {
        return glm::quat(q.w, q.x, q.y, q.z);
    }

    FLOW_INLINE static glm::mat4 assimptoMat4(const aiMatrix4x4& m) {
        return glm::mat4(
            m.a1, m.a2, m.a3, m.a4,
            m.b1, m.b2, m.b3, m.b4,
            m.c1, m.c2, m.c3, m.c4,
            m.d1, m.d2, m.d3, m.d4
        );
    }

}
