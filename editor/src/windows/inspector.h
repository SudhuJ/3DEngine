#pragma once

#include "controls/camera.h"
#include "controls/enttinfo.h"
#include "controls/transform.h"
#include "controls/directlight.h"

#include "context/context.h"
#include "context/widget.h"

struct InspectorWindow : IWidget {
    FLOW_INLINE InspectorWindow(GuiContext* context) : IWidget(context) {
        m_Widgets.push_back(context->CreateWidget<InfoControl>());
        m_Widgets.push_back(context->CreateWidget<TransformControl>());
        m_Widgets.push_back(context->CreateWidget<CameraControl>());
        m_Widgets.push_back(context->CreateWidget<DirectLightControl>());
    }

    FLOW_INLINE void onShow(GuiContext* context) override {
        if (ImGui::Begin(ICON_FA_CIRCLE_INFO "\tInspector")) {
            for (auto& widget : m_Widgets) {
                widget->onShow(context);
            }
        }
        ImGui::End();
    }

    FLOW_INLINE void onSelect(Entity entity) override {
        for (auto& widget : m_Widgets) {
            widget->onSelect(entity);
        }
    }

    private:
        std::vector<Widget> m_Widgets;
};
