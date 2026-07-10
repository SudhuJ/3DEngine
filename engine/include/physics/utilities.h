#pragma once
#include "helpers.h"

namespace flow {
    struct RigidBody3D {
        FLOW_INLINE RigidBody3D(const RigidBody3D&) = default;
        FLOW_INLINE RigidBody3D() = default;

        PxRigidActor* actor = nullptr;

        float Density = 1.0f;
        bool Dynamic = false;
    };

    enum ColliderType : uint8_t {
        UNKNOWN = 0,
        CAPSULE,
        SPHERE,
        BOX,
        MESH
    };


    struct Collider3D {
        FLOW_INLINE Collider3D(const Collider3D&) = default;
        FLOW_INLINE Collider3D() = default;

        PxMaterial* Material = nullptr;
        float DynamicFriction = 0.5f;
        float StaticFriction = 0.0f;
        float Restitution = 0.1f;

        PxConvexMeshGeometry Mesh;
        PxShape* Shape = nullptr;
        ColliderType Type;
    };
}
