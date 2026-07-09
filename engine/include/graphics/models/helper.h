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
        glm::mat4 to;
        to[0][0] = m.a1; to[1][0] = m.a2; to[2][0] = m.a3; to[3][0] = m.a4;
        to[0][1] = m.b1; to[1][1] = m.b2; to[2][1] = m.b3; to[3][1] = m.b4;
        to[0][2] = m.c1; to[1][2] = m.c2; to[2][2] = m.c3; to[3][2] = m.c4;
        to[0][3] = m.d1; to[1][3] = m.d2; to[2][3] = m.d3; to[3][3] = m.d4;
        return to;
    }

}
