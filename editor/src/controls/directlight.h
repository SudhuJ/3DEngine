#pragma once

#include  "IControl.h"

struct DirectLightControl : IControl<directLightComponent> {
    FLOW_INLINE DirectLightControl(GuiContext* context) : IControl(context) {
        setTitle(ICON_FA_SUN "\tDirect Light");
    }

    FLOW_INLINE void onBody(GuiContext* context, Entity& entity) {
        auto& data = entity.template Get<directLightComponent>().Light;
        InputFloat("Intensity", &data.Intensity);
        InputColor("Radiance", &data.Radiance.x);
        // InputFloat("FOV", &data.FOV);
    }

    FLOW_INLINE void onMenu(GuiContext* context, Entity& entity) {
        if (ImGui::Selectable(ICON_FA_RECYCLE "\tReset")) {
            entity.template Get<directLightComponent>() = {};
        }
    }

};
