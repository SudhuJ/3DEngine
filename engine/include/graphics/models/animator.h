#pragma once
#include "animation.h"

namespace flow {
    struct Animator {
        FLOW_INLINE JointMatrices* Animate(float deltaTime) {
            if (m_Sequence < m_Animations.size()) {
                m_Time += m_Animations[m_Sequence].Speed * deltaTime;
                m_Time = fmod(m_Time, m_Animations[m_Sequence].Duration);
                UpdateJoints(m_Root, glm::identity<glm::mat4>());
            }
            return &m_Transforms;
        }

        FLOW_INLINE glm::mat4 Interpolate(const keyFrame& prev, const keyFrame& next, float progress) {
            auto T = glm::translate(glm::mat4(1.0f), glm::mix(prev.Position, next.Position, progress));
            auto R = glm::toMat4(glm::normalize(glm::slerp(prev.Rotation, next.Rotation, progress)));
            auto S = glm::scale(glm::mat4(1.0f), glm::mix(prev.Scale, next.Scale, progress));
            return T * R * S;
        }

        FLOW_INLINE void getPrevNextFrames(Joint& joint, keyFrame& prev, keyFrame& next) {
            for (uint32_t i=1u; i<joint.Keys.size(); i++) {
                if (joint.Keys[i].timeStamp > m_Time) {
                    prev = joint.Keys[i-1];
                    next = joint.Keys[i];
                    return;
                }
            }
        }

        FLOW_INLINE void UpdateJoints(Joint& joint, const glm::mat4& parentTransform) {
            keyFrame prev, next;
            getPrevNextFrames(joint, prev, next);
            float progress = (m_Time - prev.timeStamp) / (next.timeStamp - prev.timeStamp);
            glm::mat4 transform = parentTransform * Interpolate(prev, next, progress);
            m_Transforms[joint.Index] = transform * m_GlobalTransform * joint.Offset;
            for (auto& child : joint.Children) {
                UpdateJoints(child, transform);
            }
        }

        private:
            friend struct skeletalModel;
            std::vector<Animation> m_Animations;
            std::vector<glm::mat4> m_Transforms;
            glm::mat4 m_GlobalTransform;
            JointMatrices m_Joints;

            int32_t m_Sequence = 0;
            float m_Time = 0.0f;
            Joint m_Root;
    };
}
