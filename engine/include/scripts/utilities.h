#pragma once
#include "helpers.h"
#include <entt/entt.hpp>

namespace flow {
    struct Script {
        using entityID = entt::entity;

        FLOW_INLINE Script (sol::table handle, const std::string& name):
        m_Handle(handle), m_Name(name) {}

        FLOW_INLINE bool Valid() {
            return m_Handle.valid();
        }

        FLOW_INLINE const std::string& Name() {
            return m_Name;
        }

        FLOW_INLINE void onResize(int32_t width, int32_t height) {
            if (m_Handle.valid()) {
                m_Handle["onResize"](m_Handle, width, height);
            }
        }

        FLOW_INLINE void onMouseDown(int32_t button) {
            if (m_Handle.valid()) {
                m_Handle["onMouseDown"](m_Handle, button);
            }
        }

        FLOW_INLINE void onCollision(entityID other) {
            if (m_Handle.valid()) {
                m_Handle["onCollision"](m_Handle, other);
            }
        }

        FLOW_INLINE void onKeyDown(int32_t key) {
            if (m_Handle.valid()) {
                m_Handle["onKeyDown"](m_Handle, key);
            }
        }

        FLOW_INLINE void onUpdate(float dt) {
            if (m_Handle.valid()) {
                m_Handle["onUpdate"](m_Handle, dt);
            }
        }

        FLOW_INLINE void onDestroy() {
            if (m_Handle.valid()) {
                m_Handle["onDestroy"](m_Handle);
            }
        }

        FLOW_INLINE void onStart() {
            if (m_Handle.valid()) {
                m_Handle["onStart"](m_Handle);
            }
        }

        private:
            friend struct ScriptContext;
            sol::table m_Handle;
            std::string m_Name;
    };
    using LuaScript = std::unique_ptr<Script>;
}
