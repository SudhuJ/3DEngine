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

            // int fw = 0, fh = 0;
            // glfwGetFramebufferSize(context->GetWindowHandle(), &fw, &fh);

            // if (m_Viewport.x != fw || m_Viewport.y != fh) {
            //     context->postEvent<windowResizeEvent>(fw, fh);
            //     m_Viewport = ImVec2((float)fw, (float)fh);
            // }

            auto io = ImGui::GetIO();

            // && !context->getRuntime()
            if (ImGui::IsWindowHovered() ) {
                // keyboard fly (relative to view)
                float speed = 20.0f;
                float dt = io.DeltaTime;
                float sensitivity = 50;
                float orbitSensitivity = 10;

                context->enttView<Entity, cameraComponent>([&] (auto entity, auto& comp) {
                    auto& transform = entity.template Get<transformComponent>().Transform;
                    glm::quat orientation = glm::quat(glm::radians(transform.Rotate));
                    glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
                    glm::vec3 right   = orientation * glm::vec3(1.0f, 0.0f,  0.0f);
                    glm::vec3 move(0.0f);
                    auto* win = context->GetWindowHandle();
                    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) move += forward;
                    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) move -= forward;
                    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) move += right;
                    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) move -= right;
                    if (glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS) move += glm::vec3(0.0f, 1.0f, 0.0f);
                    if (glfwGetKey(win, GLFW_KEY_Q) == GLFW_PRESS) move -= glm::vec3(0.0f, 1.0f, 0.0f);
                    if (glm::length(move) > 0.0f)
                        transform.Translate += glm::normalize(move) * speed * dt;
                    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                        transform.Rotate.x += io.MouseDelta.y * io.DeltaTime * orbitSensitivity;
                        transform.Rotate.x = glm::clamp(transform.Rotate.x, -89.0f, 89.0f);
                        transform.Rotate.y += io.MouseDelta.x * io.DeltaTime * orbitSensitivity;
                    }
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
