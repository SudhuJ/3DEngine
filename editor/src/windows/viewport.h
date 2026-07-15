#pragma once

#include "context/context.h"

struct ViewportWindow : IWidget {
  FLOW_INLINE ViewportWindow(GuiContext* context) : IWidget(context) {
      // m_Frame = (ImTextureID)context->GetSceneFrame();
  }

FLOW_INLINE void onShow(GuiContext* context) override {
    if (ImGui::Begin(ICON_FA_IMAGE "\tViewport")) {
        ImGui::BeginChild("Frame"); {
            const ImVec2 size = ImGui::GetWindowContentRegionMax();
            m_Frame = (ImTextureID)context->GetSceneFrame();
            ImGui::Image(m_Frame, size, ImVec2(0, 1), ImVec2(1, 0));

            if (m_Viewport.x != size.x || m_Viewport.y != size.y) {
                int32_t width = (int32_t)size.x, height=(int32_t)size.y;
                context->postEvent<windowResizeEvent>(width, height);
                m_Viewport = size;
            }

            auto io = ImGui::GetIO();

            if (ImGui::IsWindowHovered()) {
                float sensitivity = 50;
                context->enttView<Entity, cameraComponent>([&] (auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    transform.Translate.z -= (io.MouseWheel * io.DeltaTime * sensitivity);
                });
            }

            if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                float sensitivity = 10;
                context->enttView<Entity, cameraComponent>([&] (auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    transform.Rotate.x += io.MouseDelta.y * io.DeltaTime * sensitivity;
                    transform.Rotate.x = glm::clamp(transform.Rotate.x, -89.0f, 89.0f);
                    transform.Rotate.y += io.MouseDelta.x * io.DeltaTime * sensitivity;
                });
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

FLOW_INLINE void onSelect(Entity entity) override {

}

  private:
      ImTextureID m_Frame = 0;
      ImVec2 m_Viewport;
};
