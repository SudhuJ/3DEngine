#pragma once

#include "IControl.h"

struct TransformControl : IControl<transformComponent> {
    FLOW_INLINE TransformControl(GuiContext* context): IControl(context) {
		setTitle(ICON_FA_LOCATION_ARROW "\tTransform");
	}

    FLOW_INLINE void onBody(GuiContext* context, Entity& entity) {
        auto& data = entity.template Get<transformComponent>().Transform;
        InputVec3("Translate", &data.Translate);
        InputVec3("Rotation", &data.Rotate);
        InputVec3("Scale", &data.Scale);
    }

    FLOW_INLINE void onMenu(GuiContext* context, Entity& entity) {
        if (ImGui::Selectable(ICON_FA_RECYCLE "\tReset")) {
            entity.template Get<transformComponent>() = {};
        }
    }
};
