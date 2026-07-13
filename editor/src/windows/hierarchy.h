#pragma once

#include "context/context.h"

struct HierarchyWindow : IWidget {
  FLOW_INLINE HierarchyWindow(GuiContext* context) : IWidget(context) {
      // m_Frame = (ImTextureID)context->GetSceneFrame();
  }

FLOW_INLINE void onShow(GuiContext* context) override {
    if (ImGui::Begin(ICON_FA_CUBES "\tHeirarchy")) {
        context->enttView<Entity, infoComponent>([&] (auto entity, auto& info) {
            bool isSelected = (m_Selected.ID() == entity.ID());
            ImGui::PushID((int32_t)entity.ID());

            std::string label = ICON_FA_CUBE "\t" + info.Name;

            if(entity.template Has<directLightComponent>())
                label = ICON_FA_SUN "\t" + info.Name;
            if(entity.template Has<cameraComponent>())
                label = ICON_FA_VIDEO "\t" + info.Name;
            if(entity.template Has<rigidBodyComponent>())
                label = ICON_FA_RING "\t" + info.Name;

            if (ImGui::Selectable(label.c_str(), &isSelected)) {
                context->postEvent<SelectEvent>(entity);
            }

            ImGui::PopID();
        });
    }
    ImGui::End();
}

FLOW_INLINE void onSelect(Entity entity) override {
    m_Selected = entity;
}

  private:
      Entity m_Selected;
};
