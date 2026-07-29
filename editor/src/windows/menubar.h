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
                if (ImGui::MenuItem(ICON_FA_STORE "Save")) {
                    context->getAppContext()->Serializer->Serialize(
                    context->getAppContext()->Scene, "resources/projects/scene.yaml");
                    context->getAppContext()->Serializer->Serialize(
                    *context->getAppContext()->Assets, "resources/projects/assets.yaml");
                }
                if (ImGui::MenuItem(ICON_FA_UPLOAD "Load")) {
                    context->getAppContext()->Scene.clear();
                    context->getAppContext()->Serializer->Deserialize(
                    context->getAppContext()->Scene, "resources/projects/scene.yaml");
                }
                if (ImGui::MenuItem(ICON_FA_PARACHUTE_BOX "Export")) {
                    // Serialize + copy game binary + resources to output dir
                }
                if (ImGui::MenuItem(ICON_FA_DOOR_CLOSED " Exit")) {
                    glfwSetWindowShouldClose(context->GetWindowHandle(), true);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Scene")) {
                if (ImGui::MenuItem(ICON_FA_ARROW_DOWN_WIDE_SHORT " Add Entity")) {
                    auto entity  = context->createEntt<Entity>();
                    entity.template Attach<transformComponent>();
                    entity.template Attach<infoComponent>().Name  = "New Entity";
                }

                // if (ImGui::MenuItem(ICON_FA_BACKWARD " Undo", "Ctrl+Z")) {

                // }
                // if (ImGui::MenuItem(ICON_FA_BACKWARD " Redo", "Ctrl+Y")) {

                // }

                ImGui::Separator();
                if (ImGui::MenuItem(ICON_FA_SCISSORS " Cut", "Ctrl+X")) {}
                if (ImGui::MenuItem(ICON_FA_CLONE " Copy", "Ctrl+C")) {}
                if (ImGui::MenuItem(ICON_FA_TRASH " Delete", "Delete")) {
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Settings")) {
                if (ImGui::BeginMenu(ICON_FA_PALETTE " Theme")) {
                    if (ImGui::MenuItem("Dark"))   ImGui::StyleColorsDark();
                    if (ImGui::MenuItem("Light"))  ImGui::StyleColorsLight();
                    if (ImGui::MenuItem("Classic")) ImGui::StyleColorsClassic();
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Extra")) {
                if (ImGui::BeginPopupModal("Help", &showHelp, ImGuiWindowFlags_NoResize)) {
                    ImGui::Text("Flow Engine — Controls");
                    ImGui::Separator();
                    ImGui::BulletText("W/A/S/D    — Move camera");
                    ImGui::BulletText("E/Q        — Up / Down");
                    ImGui::BulletText("Mouse Wheel — Zoom");
                    ImGui::BulletText("Left Drag  — Orbit");
                    ImGui::BulletText("Ctrl+Z     — Undo (TODO)");
                    ImGui::BulletText("Ctrl+S     — Save Project");
                    ImGui::BulletText("Ctrl+O     — Open Project");
                    ImGui::Separator();
                    if (ImGui::Button("Close", ImVec2(100, 0))) { showHelp = false; ImGui::CloseCurrentPopup(); }
                    ImGui::EndPopup();
                }
                if (ImGui::MenuItem(ICON_FA_QUESTION "About")) {}
                ImGui::EndMenu();
            }
            if (showAbout) ImGui::OpenPopup("About");
            if (ImGui::BeginPopupModal("About", &showAbout, ImGuiWindowFlags_NoResize)) {
                ImGui::Text("Flow Engine v0.1");
                ImGui::Separator();
                ImGui::Text("C++17 game engine with OpenGL 4.5, PhysX, Lua scripting.");
                ImGui::Separator();
                ImGui::Text("Powered by ImGui, EnTT, Assimp, sol2, yaml-cpp.");
                if (ImGui::Button("Close", ImVec2(100, 0))) { showAbout = false; ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }
            if (ImGui::BeginMenu("Play")) {
                auto& ctx = *context->getAppContext();
                if (ctx.Mode == engineMode::EDITOR) {
                    if (ImGui::MenuItem(ICON_FA_PLAY " Play")) {
                        context->getAppContext()->sceneSnapshot = context->getAppContext()->Serializer->Serialize(context->getAppContext()->Scene);
                        context->getAppContext()->StartRuntime();
                        ctx.Mode = engineMode::PLAY_IN_EDITOR;
                        glfwSetInputMode(context->GetWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    }
                }
                else {
                    if (ImGui::MenuItem(ICON_FA_STOP " Stop")) {
                        context->getAppContext()->StopRuntime();
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::PopStyleVar();
    }

    private:
        bool showHelp = true;
        bool showAbout = false;
};
