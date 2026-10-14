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
    }
};
