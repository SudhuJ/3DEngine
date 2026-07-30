#pragma once

#include "IControl.h"

struct SpawnControl : IControl<charControllerComponent> {
    FLOW_INLINE SpawnControl(GuiContext* context): IControl(context) {
		setTitle(ICON_FA_ANCHOR_LOCK "\tSpawn");
	}

    FLOW_INLINE void onBody(GuiContext* context, Entity& entity) {
        auto& ctrl = entity.template Get<charControllerComponent>().Controller;
        InputVec3("Position", &ctrl.SpawnPosition);
        InputFloat("Yaw", &ctrl.SpawnYaw);
    }

    FLOW_INLINE void onMenu(GuiContext* context, Entity& entity) {
        if (ImGui::Selectable(ICON_FA_RECYCLE "\tReset")) {
            auto& ctrl = entity.template Get<charControllerComponent>().Controller;
            ctrl.SpawnPosition = glm::vec3(0.0f, 0.0f, 10.0f);
            ctrl.SpawnYaw = 0.0f;
        }
    }
};
