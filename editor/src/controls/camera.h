#pragma once

#include  "IControl.h"

struct CameraControl : IControl<cameraComponent> {
    FLOW_INLINE CameraControl(GuiContext* context) : IControl(context) {
        setTitle(ICON_FA_VIDEO "\tCamera");
    }

    FLOW_INLINE void onBody(GuiContext* context, Entity& entity) {
        auto& data = entity.template Get<cameraComponent>().Camera;
        InputFloat("Near", &data.nearPlane);
        InputFloat("Far", &data.farPlane);
        InputFloat("FOV", &data.FOV);
    }

    FLOW_INLINE void onMenu(GuiContext* context, Entity& entity) {
        if (ImGui::Selectable(ICON_FA_RECYCLE "\tReset")) {
            entity.template Get<cameraComponent>() = {};
        }
    }

};
