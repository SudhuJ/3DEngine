#pragma once
#include "../auxillaries/ECS.h"

namespace flow {
    enum class PxEvent {
        UNKOWN = 0,
        CONTACT_BEGIN,
        CONTACT,
        TRIGGER,
        SLEEP,
        WAKE
    };

    struct PxPayload {
        entityID Entity1 = NENTT;
        entityID Entity2 = NENTT;
        PxEvent Event = PxEvent::UNKOWN;
    };

    using PxCallbackFunction = std::function<void(PxPayload&)>;

    struct PxEventCallback : public PxSimulationEventCallback {

        FLOW_INLINE void onContact(const PxContactPairHeader& header, const
            PxContactPair* pairs, PxU32 nbPairs) override {
            // collision actors
            auto actor1 = header.actors[0];
            auto actor2 = header.actors[1];

            // check if actors
            if (m_Callback && actor1 && actor2) {
            PxPayload event;
            event.Event = PxEvent::CONTACT;
            event.Entity1 = actor1->userData ? *static_cast<entityID*>(actor1->userData) : NENTT;
            event.Entity2 = actor2->userData ? *static_cast<entityID*>(actor2->userData) : NENTT;
            m_Callback(event);
            }

            FLOW_DEBUG("onContact Event!");
        }

        FLOW_INLINE void onTrigger(PxTriggerPair* pairs, PxU32 nbPairs) override {
            for (PxU32 i = 0; m_Callback && i < nbPairs; ++i) {
                const PxRigidActor* actor1 = pairs[i].triggerActor;
                const PxRigidActor* actor2 = pairs[i].otherActor;
                if (actor1 && actor2) {
                    PxPayload event;
                    event.Event = PxEvent::TRIGGER;
                    event.Entity1 = actor1->userData ? *static_cast<entityID*>(actor1->userData) : NENTT;
                    event.Entity2 = actor2->userData ? *static_cast<entityID*>(actor2->userData) : NENTT;
                    m_Callback(event);
                }
            }
            FLOW_DEBUG("onTrigger Event!");
        }

        FLOW_INLINE void onAdvance(const PxRigidBody*const* bodyBuffer,
            const PxTransform* poseBuffer, const PxU32  count) override {
            FLOW_DEBUG("onAdvance Event!");
        }

        FLOW_INLINE void onSleep(PxActor** actors, PxU32 count) override {
            FLOW_DEBUG("onSleep Event!");
        }

        FLOW_INLINE void onWake(PxActor** actors, PxU32 count) override {
            FLOW_DEBUG("onWake Event!");
        }

        FLOW_INLINE void onConstraintBreak(PxConstraintInfo* constraint, PxU32 nbContacts) override {
            FLOW_DEBUG("onConstraintBreak Event!");
        }

        private:
            PxCallbackFunction m_Callback;
            friend struct physicsContext;
    };
}
