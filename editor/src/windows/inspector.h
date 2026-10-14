#pragma once

#include "controls/camera.h"
#include "controls/enttinfo.h"
#include "controls/transform.h"
#include "controls/directlight.h"
#include "controls/spawn.h"
#include "controls/model.h"
#include "controls/rigidbody.h"

#include "context/context.h"
#include "context/widget.h"

struct InspectorWindow : IWidget {
    FLOW_INLINE InspectorWindow(GuiContext* context) : IWidget(context) {
        m_Widgets.push_back(context->CreateWidget<InfoControl>());
        m_Widgets.push_back(context->CreateWidget<TransformControl>());
        m_Widgets.push_back(context->CreateWidget<ModelControl>());
        m_Widgets.push_back(context->CreateWidget<RigidBodyControl>());
        m_Widgets.push_back(context->CreateWidget<ColliderControl>());
        m_Widgets.push_back(context->CreateWidget<CameraControl>());
        m_Widgets.push_back(context->CreateWidget<DirectLightControl>());
        m_Widgets.push_back(context->CreateWidget<SpawnControl>());
    }

    FLOW_INLINE void onShow(GuiContext* context) override {
        if (ImGui::Begin(ICON_FA_CIRCLE_INFO "\tInspector")) {
            for (auto& widget : m_Widgets) {
                widget->onShow(context);
            }
            ShowAddComponent(context);
        }
        ImGui::End();
    }

    FLOW_INLINE void onSelect(Entity entity) override {
        m_Selected = entity;
        for (auto& widget : m_Widgets) {
            widget->onSelect(entity);
        }
    }

    private:
        // lists the components the selected entity is missing
        FLOW_INLINE void ShowAddComponent(GuiContext* context) {
            if (!m_Selected.template Has<infoComponent>()) return;

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            // physics actors are created when Play starts, so components are added in edit mode only
            ImGui::BeginDisabled(context->getAppContext()->Mode != engineMode::EDITOR);
            if (InputButton(ICON_FA_PLUS "\tAdd Component", ImVec2(-1.0f, 0.0f))) {
                ImGui::OpenPopup("AddComponent");
            }
            ImGui::EndDisabled();

            if (!ImGui::BeginPopup("AddComponent")) return;
            bool any = false;
            if (!m_Selected.template Has<transformComponent>()) {
                any = true;
                if (ImGui::Selectable(ICON_FA_LOCATION_ARROW "\tTransform")) {
                    m_Selected.template Attach<transformComponent>();
                }
            }
            if (!m_Selected.template Has<modelComponent>()) {
                any = true;
                if (ImGui::Selectable(ICON_FA_CUBE "\tModel")) {
                    EnsureTransform();
                    m_Selected.template Attach<modelComponent>();
                }
            }
            if (!m_Selected.template Has<rigidBodyComponent>()) {
                any = true;
                if (ImGui::Selectable(ICON_FA_RING "\tStatic Rigid Body")) AddBody(false);
                if (ImGui::Selectable(ICON_FA_RING "\tDynamic Rigid Body")) AddBody(true);
            }
            if (!m_Selected.template Has<colliderComponent>()) {
                any = true;
                if (ImGui::Selectable(ICON_FA_CUBES "\tCollider")) {
                    EnsureTransform();
                    m_Selected.template Attach<colliderComponent>();
                }
            }
            if (!any) {
                ImGui::TextDisabled("All components added.");
            }
            ImGui::EndPopup();
        }

        // a body is only simulated with a collider, so attach a box one alongside it
        FLOW_INLINE void AddBody(bool dynamic) {
            EnsureTransform();
            m_Selected.template Attach<rigidBodyComponent>().RigidBody.Dynamic = dynamic;
            if (!m_Selected.template Has<colliderComponent>()) {
                m_Selected.template Attach<colliderComponent>().Collider.Type = BOX;
            }
        }

        FLOW_INLINE void EnsureTransform() {
            if (!m_Selected.template Has<transformComponent>()) {
                m_Selected.template Attach<transformComponent>();
            }
        }

        std::vector<Widget> m_Widgets;
        Entity m_Selected;
};
