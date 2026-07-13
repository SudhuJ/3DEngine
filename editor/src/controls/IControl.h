#pragma once

#include "../context/context.h"
#include "../context/helpers.h"

template <typename Component>
struct IControl : public IWidget {
    FLOW_INLINE IControl(GuiContext* context) : IWidget(context) {}
    FLOW_INLINE virtual ~IControl() = default;

    FLOW_INLINE void onSelect(Entity entity) override {
        m_Entity = entity;
    }

    FLOW_INLINE void setTitle(const char* title) override {
        m_Title = title;
    }

    FLOW_INLINE void onShow(GuiContext* context) override {
        if(!m_Entity.template Has<Component>()) {
            return;
        }
        ImGui::PushID(m_Title);
        static const auto flags = ImGuiTreeNodeFlags_DefaultOpen |
            ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
            ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowOverlap;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 4.0f));
        bool isCollapsed = ImGui::CollapsingHeader(m_Title, flags);
        ImGui::PopStyleVar();

        USE_SMALL_FONT();

        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 30.0f);

        if(InputButton(ICON_FA_GEAR)) {
            ImGui::OpenPopup(m_Title);
        }

        if (ImGui::BeginPopup(m_Title)) {
            ImGui::Text(m_Title);
            ImGui::Separator();
            onMenu(context, m_Entity);
            ImGui::EndPopup();
        }

        if(isCollapsed) {
            ImGui::Spacing();
            ImGui::Indent(5.0f);
            onBody(context, m_Entity);
            ImGui::Spacing();
            ImGui::Unindent(5.0f);
        }
        ImGui::PopID();
        USE_REGULAR_FONT();
    }

    protected:
        FLOW_INLINE void virtual onMenu(GuiContext*, Entity&) {}
        FLOW_INLINE void virtual onBody(GuiContext*, Entity&) {}

    private:
        const char* m_Title;
        Entity m_Entity;
};
