#pragma once
#include "callback.h"
#include "utilities.h"
#include "common/event.h"

namespace flow {
    struct physicsContext {
        FLOW_INLINE physicsContext() {
            m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
            if (!m_Foundation) {
                FLOW_ERROR("Error initializing PhysX m_Foundation");
                return;
            }

            m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale());
            if (!m_Physics) {
                FLOW_ERROR("Error initializing PhysX m_Physics");
                m_Foundation->release();
                return;
            }

            m_Dispatcher = PxDefaultCpuDispatcherCreate(2);
            if (!m_Dispatcher) {
                FLOW_ERROR("Error initializing PhysX m_Dispatcher");
                return;
            }

            // Create a scene description
            PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
            sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
            sceneDesc.cpuDispatcher = m_Dispatcher;
            sceneDesc.filterShader = CustomFilterShader;
            sceneDesc.simulationEventCallback = &m_EventCallback;

            m_Scene = m_Physics->createScene(sceneDesc);

            if (!m_Scene) {
                FLOW_ERROR("Error initializing PhysX m_Scene");
                m_Physics->release();
                m_Foundation->release();
                return;
            }
        }

        FLOW_INLINE ~physicsContext() {
            if (m_Scene) { m_Scene->release(); }
            if (m_Physics) { m_Physics->release(); }
            if (m_Dispatcher) { m_Dispatcher->release(); }
            if (m_Foundation) { m_Foundation->release(); }
        }

        FLOW_INLINE void AddRigidBody(Entity& entity) {
            auto& transform = entity.template Get<transformComponent>().Transform;
            auto& body = entity.template Get<rigidBodyComponent>().RigidBody;
            bool hasCollider = entity.template Has<colliderComponent>();

            PxTransform pose(Vec3ToPx(transform.Translate));
            glm::quat rot(transform.Rotate);
            pose.q = PxQuat(rot.x, rot.y, rot.z, rot.w);

            if (entity.template Has<colliderComponent>()) {
                auto& collider = entity.template Get<colliderComponent>().Collider;
                collider.Material = m_Physics->createMaterial(collider.StaticFriction,
                    collider.DynamicFriction, collider.Restitution);

                if (collider.Type == BOX) {
                    PxBoxGeometry box(Vec3ToPx(transform.Scale/2.0f));
                    collider.Shape = m_Physics->createShape(box, *collider.Material);
                }
                else if (collider.Type == SPHERE) {
                    PxSphereGeometry sphere(transform.Scale.x/2.0f);
                    collider.Shape = m_Physics->createShape(sphere, *collider.Material);
                }
                // else if (collider.Type == Collider3D::BOX) {
                //     PxBoxGeometry box(Vec3ToPx(transform.Scale/2.0f));
                //     collider.Shape = m_Physics->createShape(box, *collider.Material);
                // }
                else {
                    FLOW_ERROR("Error creating collider: unknown type.");
                    return;
                }

                // create actor instance
                if (body.Dynamic) {
                    body.actor = PxCreateDynamic(*m_Physics, pose, *collider.Shape, body.Density);
                    body.actor->setActorFlag(PxActorFlag::eSEND_SLEEP_NOTIFIES, true);
                }
                else if (!body.Dynamic) {
                    body.actor = PxCreateStatic(*m_Physics, pose, *collider.Shape);
                }
                if (!body.actor) {
                    FLOW_ERROR("Error creating dynamic actor");
                    return;
                }
                body.actor->userData = new entityID(entity.ID());
                m_Scene->addActor(*body.actor);
            }
            else {
                if (body.Dynamic) {
                    body.actor = m_Physics->createRigidDynamic(pose);
                }
                else if (!body.Dynamic) {
                    body.actor = m_Physics->createRigidStatic(pose);
                }
            }
        };

        FLOW_INLINE void setEventCallback(PxCallbackFunction&& callback) {
            m_EventCallback.m_Callback = callback;
        }

        FLOW_INLINE PxConvexMeshGeometry CookMesh(const meshData<shadedVertex>& data) {
            std::vector<PxVec3> vertices;
            for (const auto& vertex : data.vertices) {
                vertices.push_back(Vec3ToPx(vertex.Position));
            }

            PxConvexMeshDesc meshDesc;
            meshDesc.points.count = vertices.size();
            meshDesc.points.stride = sizeof(PxVec3);
            meshDesc.points.data = vertices.data();

            meshDesc.indices.data = data.indices.data();
            meshDesc.indices.count = data.indices.size();

            meshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

            PxCookingParams cookingParams = PxCookingParams(PxTolerancesScale());
            PxCooking* cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, cookingParams);
            PxConvexMeshCookingResult::Enum result;
            PxConvexMesh* convexMesh = cooking->createConvexMesh(meshDesc,
            m_Physics->getPhysicsInsertionCallback(), &result);
            PxConvexMeshGeometry convexMeshGeometry(convexMesh);

            cooking->release();
            return convexMeshGeometry;
        }

        FLOW_INLINE void Simulate(uint32_t step, float dt) {
            for (int i=0; i<step; i++) {
                m_Scene->simulate(dt);
                m_Scene->fetchResults(true);
            }
        }

        private:
            static PxFilterFlags CustomFilterShader(
                PxFilterObjectAttributes attributes0, PxFilterData filterData0,
                PxFilterObjectAttributes attributes1, PxFilterData filterData1,
                PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize) {
                pairFlags |= PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eTRIGGER_DEFAULT;
                return PxFilterFlag::eDEFAULT;
            }

        private:
            PxDefaultErrorCallback m_ErrorCallback;
            PxDefaultCpuDispatcher* m_Dispatcher;
            PxEventCallback m_EventCallback;
            PxDefaultAllocator m_Allocator;
            PxFoundation* m_Foundation;
            PxPhysics* m_Physics;
            PxScene* m_Scene;
    };
}
