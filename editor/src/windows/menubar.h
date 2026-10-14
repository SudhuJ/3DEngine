#pragma once

#include "context/context.h"
#include <portable-file-dialogs.h>

// pfd::select_folder::result() calls back() on an empty string when the dialog
// is cancelled (aborts with libstdc++ assertions); read the raw result instead
struct FolderDialog : pfd::select_folder {
    using pfd::select_folder::select_folder;

    FLOW_INLINE std::string result() {
        auto ret = m_async->result();
        if (!ret.empty() && ret.back() == '\n') ret.pop_back();
        return ret;
    }
};

struct MenuBarWindow : IWidget {

    FLOW_INLINE MenuBarWindow(GuiContext* context) : IWidget(context) {
        // m_Frame = (ImTextureID)context->GetSceneFrame();
    }

    FLOW_INLINE void onShow(GuiContext* context) override {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6,6));
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                auto& ctx = *context->getAppContext();
                if (ImGui::MenuItem(ICON_FA_FOLDER_PLUS " New Project...")) {
                    auto dir = FolderDialog("New Project Folder", FLOW_PROJECTS_DIR).result();
                    if (!dir.empty()) {
                        std::filesystem::create_directories(dir);
                        ctx.OpenProject(dir);
                        if (ctx.Scene.view<infoComponent>().empty()) {
                            ctx.SpawnDefaultScene();
                            ctx.InitSkyboxes();
                        }
                        ctx.Project->SaveProject();
                        context->postEvent<SelectEvent>(NENTT);
                    }
                }
                if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open Project...")) {
                    auto selection = pfd::open_file("Open Project", FLOW_PROJECTS_DIR,
                        { "Project file", "project.yaml" }).result();
                    if (!selection.empty()) {
                        ctx.OpenProject(std::filesystem::path(selection[0]).parent_path().string());
                        context->postEvent<SelectEvent>(NENTT);
                    }
                }
                if (ImGui::MenuItem(ICON_FA_STORE " Save", nullptr, false,
                        ctx.Mode == engineMode::EDITOR)) {
                    ctx.Project->SaveProject();
                }
                if (ImGui::MenuItem(ICON_FA_STORE " Save As...", nullptr, false,
                        ctx.Mode == engineMode::EDITOR)) {
                    auto dir = FolderDialog("Save Project As", FLOW_PROJECTS_DIR).result();
                    if (!dir.empty()) {
                        ctx.Project->ExportProject(dir, true);
                    }
                }
                if (ImGui::MenuItem(ICON_FA_PARACHUTE_BOX " Export...", nullptr, false,
                        ctx.Mode == engineMode::EDITOR)) {
                    auto dir = FolderDialog("Export Project", FLOW_PROJECTS_DIR).result();
                    if (!dir.empty()) {
                        ctx.Project->ExportProject(dir);
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
                    bool editable = context->getAppContext()->Mode == engineMode::EDITOR;
                    if (ImGui::MenuItem(ICON_FA_RING " Static Cube", nullptr, false, editable)) {
                        SpawnPhysicsCube(context, false);
                    }
                    if (ImGui::MenuItem(ICON_FA_RING " Dynamic Cube", nullptr, false, editable)) {
                        SpawnPhysicsCube(context, true);
                    }
                    if (ImGui::MenuItem(ICON_FA_BOX_OPEN " Import Model...")) {
                        auto selection = pfd::open_file("Import Model", "resources/models",
                            { "Model files", "*.obj *.fbx *.gltf *.glb *.dae" }).result();
                        if (!selection.empty()) {
                            ImportModel(context, selection[0]);
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
        // cube with a box collider; reuses the cube model asset if it's already registered
        FLOW_INLINE void SpawnPhysicsCube(GuiContext* context, bool dynamic) {
            auto& assets = *context->getAppContext()->Assets;
            const std::string cubeSrc = "resources/models/cube.fbx";
            auto model = assets.FindBySource<ModelAsset>(cubeSrc);
            if (!model) model = assets.AddModel(RandomU64(), cubeSrc);

            auto entity = context->createEntt<Entity>();
            entity.template Attach<infoComponent>().Name = dynamic ? "Dynamic Cube" : "Static Cube";
            auto& transform = entity.template Attach<transformComponent>().Transform;
            transform.Translate = glm::vec3(0.0f, dynamic ? 5.0f : 0.0f, 0.0f);
            entity.template Attach<modelComponent>().Model = model->UID;
            entity.template Attach<rigidBodyComponent>().RigidBody.Dynamic = dynamic;
            entity.template Attach<colliderComponent>().Collider.Type = BOX;
            context->postEvent<SelectEvent>(entity.ID());
        }

        // registers model + material (+ textures) and spawns an entity that renders it
        FLOW_INLINE void ImportModel(GuiContext* context, const std::string& file) {
            auto& ctx = *context->getAppContext();
            auto& assets = *ctx.Assets;
            auto* project = ctx.Project->Current.get();
            auto relative = [&](const std::string& path) {
                return project ? project->MakeRelative(path) : path;
            };

            const bool hasJoints = ModelHasJoints(file);
            auto modelAsset = assets.AddModel(RandomU64(), relative(file), hasJoints);
            if (!modelAsset->Data) {
                FLOW_ERROR("Import failed: {}", file);
                return;
            }

            auto mtlAsset = assets.AddMaterial(RandomU64(), modelAsset->Name);
            if (auto* mat = modelAsset->Data->GetDefaultMaterial()) {
                mtlAsset->Data.Albedo    = mat->Albedo;
                mtlAsset->Data.Roughness = mat->Roughness;
                mtlAsset->Data.Metallic  = mat->Metallic;
                mtlAsset->Data.Occlusion = mat->Occlusion;

                // reuse a texture asset if the same file was imported before
                auto linkMap = [&](const std::string& src, AssetID& id) {
                    if (src.empty()) return;
                    auto rel = relative(src);
                    auto tex = assets.FindBySource<TextureAsset>(rel);
                    if (!tex) tex = assets.AddTexture(RandomU64(), rel);
                    id = tex->UID;
                };
                linkMap(mat->AlbedoPath,   mtlAsset->AlbedoMap);
                linkMap(mat->NormalPath,   mtlAsset->NormalMap);
                linkMap(mat->EmissivePath, mtlAsset->EmissiveMap);
                linkMap(mat->AOPath,       mtlAsset->OcclusionMap);
                linkMap(mat->RoughPath,    mtlAsset->RoughnessMap);
                linkMap(mat->MetalPath,    mtlAsset->MetallicMap);
            }
            assets.ResolveMaterialTextures(mtlAsset->UID);

            auto entity = context->createEntt<Entity>();
            entity.template Attach<infoComponent>().Name = modelAsset->Name;
            entity.template Attach<transformComponent>();
            auto& comp = entity.template Attach<modelComponent>();
            comp.Model    = modelAsset->UID;
            comp.Material = mtlAsset->UID;
            context->postEvent<SelectEvent>(entity.ID());
        }

        bool showHelp = true;
        bool showAbout = false;
        std::string m_SavedSnapshot;
        bool m_ShowExitConfirm = false;
};
