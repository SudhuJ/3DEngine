#pragma once
#include "../context/helpers.h"

struct PathModal {
    bool Open = false;
    char Path[256] = "resources/projects";

    void Show(const char* title) {
        if (Open) ImGui::OpenPopup(title);
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(420, 120));
        if (ImGui::BeginPopupModal(title, &Open, ImGuiWindowFlags_NoResize)) {
            ImGui::Text("Project folder path:");
            ImGui::InputText("##path", Path, sizeof(Path));
            ImGui::Separator();
            if (ImGui::Button("Confirm", ImVec2(100, 0))) {
                m_Callback(Path);
                Open = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(100, 0))) {
                Open = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    void SetCallback(std::function<void(const std::string&)> cb) { m_Callback = std::move(cb); }
    void OpenModal() { Open = true; }

    private:
        std::function<void(const std::string&)> m_Callback;
};
