#pragma once

#include "auxillaries/ECS.h"
#include "window/window.h"
#include "physics/context.h"
namespace flow {
    struct ScriptContext{
        FLOW_INLINE ScriptContext(entityRegistry* scene, appWindow* window) {
            m_Lua.open_libraries(sol::lib::base, sol::lib::math);

            m_Lua["inputs"] = m_Lua.require_file("inputs", "resources/scripts/engine/input.lua");
            m_Lua.require_file("core", "resources/scripts/engine/core.lua");

            m_Lua["TRANSFORM"] = TypeID<transformComponent>();
            m_Lua.new_usertype<transform3D>("transform3D",
                "Translate", &transform3D::Translate,
                "Rotate", &transform3D::Rotate,
                "Scale", &transform3D::Scale);

            m_Lua.new_usertype<glm::vec3>("Vec3",
                sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
                "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z);

            m_Lua["CONTROLLER"] = TypeID<charControllerComponent>();
            m_Lua.new_usertype<charControllerComponent>("CharController",
                "Yaw", &charControllerComponent::Yaw,
                "Pitch", &charControllerComponent::Pitch,
                "MoveSpeed", &charControllerComponent::MoveSpeed,
                "EyeOffset", &charControllerComponent::EyeOffset,
                "VerticalVelocity", &charControllerComponent::VerticalVelocity
            );

            m_Lua.new_usertype<glm::vec2>("Vec2",
                sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
                "x", &glm::vec2::x, "y", &glm::vec2::y);

            setAPIFunctions(scene, window);
        }

        FLOW_INLINE bool attachScript(Entity& entity, const std::string& name) {
            if (!m_Lua[name].valid() && !m_Lua[name]["Constructor"].valid()) {
                FLOW_ERROR("Invalid Script Name: {}", name);
                return false;
            }

            auto object =  m_Lua[name]["Constructor"](entity.ID());
            if (!object.valid()) {
                sol::error error = object;
                sol::call_status status = object.status();
                FLOW_ERROR("Failed to create script: {}", error.what());
                return false;
            }
            entity.Attach<scriptComponent>().Instance = std::make_unique<Script>(object, name);
            return true;
        }

        FLOW_INLINE std::string loadScript(const std::string& filepath) {
            auto Initializer = m_Lua["Initializer"];
            if (!Initializer.valid()) {
                FLOW_ERROR("Failed to load script. Core not initialized.");
                return "";
            }

            std::filesystem::path path(filepath);
            if (!std::filesystem::exists(path)) {
                FLOW_ERROR("Failed to load script. Invalid file path: {}", filepath);
                return "";
            }

            auto scriptName = path.stem().string();
            m_Lua[scriptName] = Initializer();
            m_Lua.script_file(filepath);
            return scriptName;
        }

        private:
            FLOW_INLINE void setAPIFunctions(entityRegistry* scene, appWindow* window) {

                m_Lua.set_function("APIGetTransform", [this, scene] (entityID entity) {
                    return std::ref(scene->get<transformComponent>(entity).Transform);
                });

                m_Lua.set_function("APIMouseDown", [this, window] (int32_t button) {
                    return window->isMouse(button);
                });

                m_Lua.set_function("APIisKeyDown", [this, window] (int32_t key) {
                    return window->isKey(key);
                });

                m_Lua.set_function("APIApplyForce", [this, scene] (entityID entity, const glm::vec3& force) {
                   if (scene->all_of<rigidBodyComponent>(entity)) {
                       auto& body = scene->get<rigidBodyComponent>(entity).RigidBody;
                       if (body.Dynamic) {
                           auto actor = static_cast<PxRigidDynamic*>(body.actor);
                           actor->addForce(Vec3ToPx(force), PxForceMode::eACCELERATION);
                           return true;
                       }
                       FLOW_ERROR("Force can't be applied to Static Body.");
                       return false;
                   }
                   FLOW_ERROR("Force can't be applied to non-physics entity.");
                   return false;
                });

                m_Lua.set_function("APIDestroy", [this, scene] (entityID entity) {
                    if (scene->valid(entity) == false) {
                        return;
                    }
                    if (scene->all_of<rigidBodyComponent>(entity)) {
                        auto& body = scene->get<rigidBodyComponent>(entity).RigidBody;
                        auto userData = static_cast<entityID*>(body.actor->userData);
                        body.actor->userData = nullptr;
                        body.actor->release();
                        FLOW_DELETE(userData);
                    }
                    if (scene->all_of<colliderComponent>(entity)) {
                        auto& collider = scene->get<colliderComponent>(entity).Collider;
                        collider.Material->release();
                        collider.Shape->release();
                    }
                    if (scene->all_of<scriptComponent>(entity)) {
                        scene->get<scriptComponent>(entity).Instance->onDestroy();
                    }
                    scene->destroy(entity);
                });

                // m_Lua.set_function("APISetControllerMove", [this, scene] (entityID entity,
                //     float x, float z) {
                //     if (scene->all_of<charControllerComponent>(entity)) {
                //         auto& comp = scene->get<charControllerComponent>(entity);
                //         comp.MoveIntent = glm::vec3(x, 0.0f, z);
                //     }
                //     return true;
                // });

                m_Lua.set_function("APISetControllerMove", [this, scene] (entityID entity,
                    float x, float z) {
                    if (scene->all_of<charControllerComponent>(entity)) {
                        auto& comp = scene->get<charControllerComponent>(entity);
                        comp.MoveIntent = glm::vec3(x, 0.0f, z);
                        FLOW_INFO("APISetControllerMove: set MoveIntent({},0,{}) on entity {}", x, z, (int)entity);
                    }
                    else {
                        FLOW_INFO("APISetControllerMove: entity {} has NO charControllerComponent!", (int)entity);
                    }
                    return true;
                });

                m_Lua.set_function("APIGetController", [this, scene] (entityID entity) {
                    return std::ref(scene->get<charControllerComponent>(entity));
                });

                m_Lua.set_function("APIGetMousePos", [window] () -> glm::vec2 {
                    double x, y;
                    glfwGetCursorPos(window->getHandle(), &x, &y);
                    return glm::vec2(static_cast<float>(x), static_cast<float>(y));
                });
            }

        private:
            sol::state m_Lua;
    };
}
