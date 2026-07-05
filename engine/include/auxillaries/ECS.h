#pragma once
#include <entt/entt.hpp>
#include "graphics/utilities/data.h"
#include "graphics/models/model.h"

namespace flow {
    using entityID = entt::entity;
    using entityRegistry = entt::registry;
    constexpr entityID NENTT = entt::null;

    struct transformComponent {
        FLOW_INLINE transformComponent(const transformComponent&) = default;
        FLOW_INLINE transformComponent() = default;
        transform3D Transform;
    };

    struct cameraComponent {
        FLOW_INLINE cameraComponent(const cameraComponent&) = default;
        FLOW_INLINE cameraComponent() = default;
        camera3D Camera;
    };

    struct enttComponent {
        FLOW_INLINE enttComponent(const enttComponent&) = default;
        FLOW_INLINE enttComponent() = default;
        std::string Name = "Untitled";
    };

    struct meshComponent {
        FLOW_INLINE meshComponent(const meshComponent&) = default;
        FLOW_INLINE meshComponent() = default;
        mesh3D Mesh;
    };

    struct Entity {
        FLOW_INLINE Entity(entityRegistry* registry, entityID entity)
        : m_Registry(registry), m_enttID(entity) {}

        FLOW_INLINE Entity(entityRegistry* registry) : m_Registry(registry) {
            m_enttID = registry->create();
        }

        FLOW_INLINE virtual ~Entity() = default;
        FLOW_INLINE Entity() = default;

        FLOW_INLINE operator entityID() {
            return m_enttID;
        }

        FLOW_INLINE operator bool() {
            return m_Registry != nullptr && m_Registry->valid(m_enttID);
        }

        FLOW_INLINE entityID ID() {
            return m_enttID;
        }

        template<typename T, typename... Args>
        FLOW_INLINE T& Attach(Args&&... args) {
            return m_Registry->get_or_emplace<T>(m_enttID, std::forward<Args>(args)...);
        }

        template<typename T>
        FLOW_INLINE void Detach() {
            m_Registry->remove<T>(m_enttID);
        }

        template<typename T>
        FLOW_INLINE void Destroy() {
            if (m_Registry != nullptr) {
                m_Registry->destroy(m_enttID);
            }
        }

        template<typename T>
        FLOW_INLINE bool Has() {
            return m_Registry != nullptr && m_Registry->all_of<T>(m_enttID);
        }

        template<typename T>
        FLOW_INLINE T& Get() {
            return m_Registry->get<T>(m_enttID);
        }


    protected:
        entityRegistry* m_Registry = nullptr;
        entityID m_enttID = NENTT;
    };

    struct modelComponent {
        FLOW_INLINE modelComponent(const modelComponent&) = default;
        FLOW_INLINE modelComponent() = default;
        model3D Model;
        PBRMaterial Material;
    };

    struct pointLightComponent {
        FLOW_INLINE pointLightComponent(const pointLightComponent&) = default;
        FLOW_INLINE pointLightComponent() = default;
        pointLight Light;
    };

    struct directLightComponent {
        FLOW_INLINE directLightComponent(const directLightComponent&) = default;
        FLOW_INLINE directLightComponent() = default;
        directLight Light;
    };

    struct spotLightComponent {
        FLOW_INLINE spotLightComponent(const spotLightComponent&) = default;
        FLOW_INLINE spotLightComponent() = default;
        spotLight Light;
    };

    struct skyboxComponent {
        FLOW_INLINE skyboxComponent(const skyboxComponent&) = default;
        FLOW_INLINE skyboxComponent() = default;
        Skybox Sky;
    };
}
