#pragma once

#include  "IControl.h"

struct InfoControl : IControl<infoComponent> {
    FLOW_INLINE InfoControl(GuiContext* context) : IControl(context) {
        setTitle(ICON_FA_INFO "\tEntity");
    }

    FLOW_INLINE void onBody(GuiContext* context, Entity& entity) {
        auto& data = entity.template Get<infoComponent>();
        InputText("Name", data.Name.data(), "Untitled", 64);
    }

    FLOW_INLINE void onMenu(GuiContext* context, Entity& entity) {
        if (ImGui::Selectable(ICON_FA_RECYCLE "\tReset")) {
            entity.template Get<infoComponent>() = {};
        }
    }

};
