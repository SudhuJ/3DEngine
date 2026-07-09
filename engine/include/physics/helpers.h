#pragma once
#include "common/core.h"
#include <PxPhysicsAPI.h>

using namespace physx;
namespace flow {
    FLOW_INLINE glm::vec3 PxToVec3(const PxVec3& vec) {
        return glm::vec3(vec.x, vec.y, vec.z);
    }
    FLOW_INLINE PxVec3 Vec3ToPx(const glm::vec3& vec) {
        return PxVec3(vec.x, vec.y, vec.z);
    }
}
