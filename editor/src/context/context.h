#pragma once

#include "widget.h"
#include "../../engine/include/application/interface.h"

struct GuiContext : appInterface {
    FLOW_INLINE virtual ~GuiContext() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        // glfwTerminate();
    }

    FLOW_INLINE virtual void onGUIStart() {}
    FLOW_INLINE virtual void onGUIFrame() {}

    FLOW_INLINE void onStart() override final {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui_ImplOpenGL3_Init(SHADER_VERSION);
        ImGui_ImplGlfw_InitForOpenGL(this->GetWindowHandle(), true);

        ImFontConfig fontConfig;
        fontConfig.MergeMode = true;
        fontConfig.PixelSnapH = true;
        static const ImWchar iconRange[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
        io.Fonts->AddFontFromFileTTF(FONT_FILE, REGULAR_FONT_SIZE);
        io.Fonts->AddFontFromFileTTF(ICON_FONT, REGULAR_FONT_SIZE, &fontConfig, iconRange);

        io.Fonts->AddFontFromFileTTF(FONT_FILE, SMALL_FONT_SIZE);
        io.Fonts->AddFontFromFileTTF(ICON_FONT, SMALL_FONT_SIZE, &fontConfig, iconRange);

        ImGui::StyleColorsDark();

        attachCallback<SelectEvent>([this] (auto e) {
            for (auto& window : m_Windows) {
                window->onSelect(toEntt<Entity>(e.EnttID));
            }
        });
        onGUIStart();
    }

    FLOW_INLINE void onUpdate() override final {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowPos(viewport->Pos);

        // Define main window flags
        static auto flags = ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_MenuBar;

        // Set window style
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);

        ImGui::Begin("MainWindow", NULL, flags);
        {
            ImGui::DockSpace(ImGui::GetID("MainDockspace"), ImVec2(0, 0),
            ImGuiDockNodeFlags_PassthruCentralNode);
            ImGui::PopStyleColor();
            ImGui::PopStyleVar(3);

            for (auto& window : m_Windows) {
                window->onShow(this);
            }

            // ImGui::ShowDemoWindow();
            onGUIFrame();
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    template<typename T, typename... Args>
    FLOW_INLINE void AttachWindow(Args&&... args) {
        FLOW_STATIC_ASSERT(std::is_base_of<IWidget, T>::value);
        auto window = std::make_unique<T>(this, std::forward<Args>(args)...);
        m_Windows.push_back(std::move(window));
    }

    template<typename T, typename... Args>
    FLOW_INLINE auto CreateWidget(Args&&... args) {
        FLOW_STATIC_ASSERT(std::is_base_of<IWidget, T>::value);
        auto widget = std::make_unique<T>(this, std::forward<Args>(args)...);
        return widget;
    }

    private:
        std::vector<Widget> m_Windows;
};
