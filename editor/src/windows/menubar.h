#pragma once

#include "context/context.h"
#include <portable-file-dialogs.h>

struct MenuBarWindow : IWidget {

    FLOW_INLINE MenuBarWindow(GuiContext* context) : IWidget(context) {
        // m_Frame = (ImTextureID)context->GetSceneFrame();
    }

    FLOW_INLINE void onShow(GuiContext* context) override {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6,6));
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem(ICON_FA_STORE "Save")) {
                    auto path = pfd::save_file("Save Project", "resources/projects/scene.yaml",
                        { "Scene file", "*.yaml" }).result();
                    if (!path.empty()) {
                        auto& ctx = *context->getAppContext();
                        std::filesystem::path scenePath(path);
                        ctx.Serializer->Serialize(ctx.Scene, scenePath.string());
                        ctx.Serializer->Serialize(*ctx.Assets, (scenePath.parent_path() / "assets.yaml").string());
                    }
                }
                if (ImGui::MenuItem(ICON_FA_UPLOAD "Load")) {
                    auto selection = pfd::open_file("Load Project", "resources/projects",
                        { "Scene file", "*.yaml" }).result();
                    if (!selection.empty()) {
                        auto& ctx = *context->getAppContext();
                        std::filesystem::path scenePath(selection[0]);
                        auto assetsPath = scenePath.parent_path() / "assets.yaml";
                        ctx.Scene.clear();
                        ctx.Assets->Reset();
                        ctx.Serializer->Deserialize(ctx.Scene, scenePath.string());
                        if (std::filesystem::exists(assetsPath)) {
                            ctx.Serializer->Deserialize(*ctx.Assets, assetsPath.string());
                        }
                    }
                }
                if (ImGui::MenuItem(ICON_FA_PARACHUTE_BOX "Export")) {
                    auto dir = pfd::select_folder("Export Project", "resources/projects").result();
                    if (!dir.empty()) {
                        auto& ctx = *context->getAppContext();
                        std::filesystem::path out(dir);
                        ctx.Serializer->Serialize(ctx.Scene, (out / "scene.yaml").string());
                        ctx.Serializer->Serialize(*ctx.Assets, (out / "assets.yaml").string());
                    }
                }
                if (ImGui::MenuItem(ICON_FA_DOOR_CLOSED " Exit")) {
                    glfwSetWindowShouldClose(context->GetWindowHandle(), true);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Scene")) {
                if (ImGui::BeginMenu(ICON_FA_ARROW_DOWN_WIDE_SHORT " Add Entity")) {
                    if (ImGui::MenuItem("Empty Entity")) {
                        auto entity = context->createEntt<Entity>();
                        entity.template Attach<transformComponent>();
                        entity.template Attach<infoComponent>().Name = "New Entity";
                    }
                    if (ImGui::MenuItem(ICON_FA_BOX_OPEN " Import Model...")) {
                        auto selection = pfd::open_file("Import Model", "resources/placeholders",
                            { "Model files", "*.obj *.fbx" }).result();
                        if (!selection.empty()) {
                            auto& assets = *context->getAppContext()->Assets;
                            auto modelAsset = assets.AddModel(RandomU64(), selection[0], false);
                            auto mtlAsset = assets.AddMaterial(RandomU64(), modelAsset->Name);
                            if (auto* mat = modelAsset->Data->GetDefaultMaterial()) {
                                mtlAsset->Data.Albedo = mat->Albedo;
                                mtlAsset->Data.Roughness = mat->Roughness;
                                mtlAsset->Data.Metallic = mat->Metallic;
                                mtlAsset->Data.Occlusion = mat->Occlusion;
                                if (!mat->AlbedoPath.empty()) {
                                    auto texAsset = assets.AddTexture(RandomU64(), mat->AlbedoPath);
                                    mtlAsset->AlbedoMap = texAsset->UID;
                                    mtlAsset->Data.AlbedoMap = texAsset->Data;
                                }
                            }
                        }
                    }
                    ImGui::EndMenu();
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
                    if (ImGui::MenuItem("Dark Gray")) ApplyDarkTheme();
                    if (ImGui::MenuItem("Light Gray")) ApplyLightTheme();
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            // if (ImGui::BeginMenu("Extra")) {
            //     if (ImGui::BeginPopupModal("Help", &showHelp, ImGuiWindowFlags_NoResize)) {
            //         ImGui::Text("Flow Engine — Controls");
            //         ImGui::Separator();
            //         ImGui::BulletText("W/A/S/D    — Move camera");
            //         ImGui::BulletText("E/Q        — Up / Down");
            //         ImGui::BulletText("Mouse Wheel — Zoom");
            //         ImGui::BulletText("Left Drag  — Orbit");
            //         ImGui::BulletText("Ctrl+Z     — Undo (TODO)");
            //         ImGui::BulletText("Ctrl+S     — Save Project");
            //         ImGui::BulletText("Ctrl+O     — Open Project");
            //         ImGui::Separator();
            //         if (ImGui::Button("Close", ImVec2(100, 0))) { showHelp = false; ImGui::CloseCurrentPopup(); }
            //         ImGui::EndPopup();
            //     }
            //     if (ImGui::MenuItem(ICON_FA_QUESTION "About")) {}
            //     ImGui::EndMenu();
            // }
            if (showAbout) ImGui::OpenPopup("About");
            if (ImGui::BeginPopupModal("About", &showAbout, ImGuiWindowFlags_NoResize)) {
                ImGui::Text("Flow Engine v0.1");
                ImGui::Separator();
                ImGui::TextWrapped("A C++17 game engine built on OpenGL 4.5 core (via glad), featuring PBR rendering, PhysX-backed physics, and Lua scripting through sol2.");
                ImGui::Separator();
                ImGui::Text("Powered by:");
                ImGui::BulletText("ImGui + GLFW      - editor UI & windowing");
                ImGui::BulletText("EnTT              - entity/component system");
                ImGui::BulletText("Assimp            - model import (OBJ/FBX/GLB)");
                ImGui::BulletText("stb               - image loading");
                ImGui::BulletText("glm               - math library");
                ImGui::BulletText("yaml-cpp          - scene & asset serialization");
                ImGui::BulletText("spdlog            - logging");
                ImGui::BulletText("magic_enum        - enum reflection");
                ImGui::BulletText("portable-file-dialogs - native file dialogs");
                ImGui::Separator();
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
        std::string m_SavedSnapshot;
        bool m_ShowExitConfirm = false;
};
