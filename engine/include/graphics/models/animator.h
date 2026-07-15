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
            if (joint.Keys.empty()) return;
            if (joint.Keys.size() == 1) {
                prev = joint.Keys[0];
                next = joint.Keys[0];
                return;
            }
            for (uint32_t i=1u; i<joint.Keys.size(); i++) {
                if (joint.Keys[i].timeStamp > m_Time) {
                    prev = joint.Keys[i-1];
                    next = joint.Keys[i];
                    return;
                }
            }
            prev = joint.Keys[joint.Keys.size() - 2];
            next = joint.Keys[joint.Keys.size() - 1];
        }

        FLOW_INLINE void UpdateJoints(Joint& joint, const glm::mat4& parentTransform) {
            keyFrame prev, next;
            getPrevNextFrames(joint, prev, next);
            if (joint.Keys.empty()) {
                m_Transforms[joint.Index] = parentTransform * m_GlobalTransform * joint.Offset;
                for (auto& child : joint.Children) {
                    UpdateJoints(child, parentTransform);
                }
                return;
            }
            float denom = next.timeStamp - prev.timeStamp;
            float progress = (denom > 0.0f) ? (m_Time - prev.timeStamp) / denom : 0.0f;
            progress = glm::clamp(progress, 0.0f, 1.0f);
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
            int32_t m_Sequence = 0;
            float m_Time = 0.0f;
            Joint m_Root;
    };
}
