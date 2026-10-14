#pragma once

#include  "IControl.h"

struct InfoControl : IControl<infoComponent> {
    FLOW_INLINE InfoControl(GuiContext* context) : IControl(context) {
        setTitle(ICON_FA_INFO "\tEntity");
    }

    FLOW_INLINE void onBody(GuiContext* context, Entity& entity) {
        auto& data = entity.template Get<infoComponent>();
        // edit through a fixed buffer: std::string::data() has no room to grow
        char buffer[64] = {};
        data.Name.copy(buffer, sizeof(buffer) - 1);
        if (InputText("Name", buffer, "Untitled", sizeof(buffer))) {
            data.Name = buffer;
        }
    }

    FLOW_INLINE void onMenu(GuiContext* context, Entity& entity) {
        if (ImGui::Selectable(ICON_FA_RECYCLE "\tReset")) {
            entity.template Get<infoComponent>() = {};
        }
        if (!entity.template Has<modelComponent>() &&
            ImGui::Selectable(ICON_FA_CUBE "\tAdd Model Component")) {
            entity.template Attach<modelComponent>();
            if (!entity.template Has<transformComponent>()) {
                entity.template Attach<transformComponent>();
            }
        }

        // physics actors are created when Play starts, so only add them in edit mode
        bool editable = context->getAppContext()->Mode == engineMode::EDITOR;
        if (editable && !entity.template Has<rigidBodyComponent>()) {
            if (ImGui::Selectable(ICON_FA_RING "\tAdd Static Body")) AddBody(entity, false);
            if (ImGui::Selectable(ICON_FA_RING "\tAdd Dynamic Body")) AddBody(entity, true);
        }
        if (editable && !entity.template Has<colliderComponent>() &&
            ImGui::Selectable(ICON_FA_CUBES "\tAdd Collider")) {
            entity.template Attach<colliderComponent>();
        }
    }

    private:
        // a body is only simulated with a collider, so attach a box one alongside it
        FLOW_INLINE void AddBody(Entity& entity, bool dynamic) {
            entity.template Attach<rigidBodyComponent>().RigidBody.Dynamic = dynamic;
            if (!entity.template Has<colliderComponent>()) {
                entity.template Attach<colliderComponent>().Collider.Type = BOX;
            }
            if (!entity.template Has<transformComponent>()) {
                entity.template Attach<transformComponent>();
            }
        }

};
