#pragma once

#include "context/context.h"

struct MenuBarWindow : IWidget {

    FLOW_INLINE MenuBarWindow(GuiContext* context) : IWidget(context) {
        // m_Frame = (ImTextureID)context->GetSceneFrame();
    }

    FLOW_INLINE void onShow(GuiContext* context) override {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6,6));
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem(ICON_FA_FILE " Open Project", "Ctrl+O")) {}
                if (ImGui::MenuItem(ICON_FA_STORE " Save Scene", "Ctrl+S")) {}
                if (ImGui::MenuItem(ICON_FA_DOOR_CLOSED " Exit", "Alt+F4")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Scene")) {
                if (ImGui::MenuItem(ICON_FA_FORWARD " Add Entity")) {
                    auto entity  = context->createEntt<Entity>();
                    entity.template Attach<transformComponent>();
                    entity.template Attach<infoComponent>().Name  = "New Entity";
                }

                if (ImGui::MenuItem(ICON_FA_FORWARD " Undo", "Ctrl+Z")) {}
                if (ImGui::MenuItem(ICON_FA_BACKWARD " Redo", "Ctrl+Y")) {}
                ImGui::Separator();
                if (ImGui::MenuItem(ICON_FA_SCISSORS " Cut", "Ctrl+X")) {}
                if (ImGui::MenuItem(ICON_FA_CLONE " Copy", "Ctrl+C")) {}
                if (ImGui::MenuItem(ICON_FA_PASTE " Paste", "Ctrl+V")) {}
                if (ImGui::MenuItem(ICON_FA_TRASH " Delete", "Delete")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Settings")) {
                if (ImGui::MenuItem(ICON_FA_PALETTE " Theme")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Extra")) {
                if (ImGui::MenuItem(ICON_FA_INFO "Help", "Ctrl+H")) {}
                if (ImGui::MenuItem(ICON_FA_QUESTION "About")) {}
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::PopStyleVar();
    }
};
