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

        FLOW_INFO("ioFonts Size {}", io.Fonts->Fonts.size());

        io.Fonts->AddFontFromFileTTF(ICON_FONT, REGULAR_FONT_SIZE, &fontConfig, iconRange);

        FLOW_INFO("ioFonts Size {}", io.Fonts->Fonts.size());

        io.Fonts->AddFontFromFileTTF(FONT_FILE, SMALL_FONT_SIZE);

        FLOW_INFO("ioFonts Size {}", io.Fonts->Fonts.size());

        io.Fonts->AddFontFromFileTTF(ICON_FONT, SMALL_FONT_SIZE, &fontConfig, iconRange);

        FLOW_INFO("ioFonts Size {}", io.Fonts->Fonts.size());

        if (io.Fonts->Fonts.size() == 0) {
            io.Fonts->AddFontDefault();
        }

        ImGui::StyleColorsDark();

        attachCallback<SelectEvent>([this] (auto e) {
            for (auto& window : m_Windows) {
                window->onSelect(toEntt<Entity>(e.EnttID));
            }
        });
        onGUIStart();
    }

    FLOW_INLINE void onUpdate() override final {
        glBindProgramPipeline(0);
        glUseProgram(0);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_SCISSOR_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

        // Force-upload the font atlas every frame. The 1.92 dynamic backend may
        // have handed us a black atlas texture; re-upload valid pixels ourselves.
        {
            ImGuiIO& io = ImGui::GetIO();
            unsigned char* px = nullptr; int w = 0, h = 0;
            io.Fonts->GetTexDataAsRGBA32(&px, &w, &h);
            if (px != nullptr && w > 0 && h > 0) {
                // Defensive: guarantee the white pixel exists so UI chrome is visible
                int wx = (int)(io.Fonts->TexUvWhitePixel.x * w);
                int wy = (int)(io.Fonts->TexUvWhitePixel.y * h);
                if (wx >= 0 && wy >= 0 && wx < w && wy < h) {
                    unsigned char* p = px + (size_t)(wy * w + wx) * 4;
                    p[0] = p[1] = p[2] = p[3] = 255;
                }

                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);   // <-- insurance: client pointer upload
                auto& atlas = *io.Fonts;
                for (int i = 0; i < atlas.TexList.Size; i++) {
                    ImTextureData* t = atlas.TexList[i];
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
                    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
                    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
                    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
                    GLuint tex = (GLuint)(uintptr_t)t->TexID;
                    if (tex == 0) glGenTextures(1, &tex);
                    glBindTexture(GL_TEXTURE_2D, tex);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    t->SetTexID((ImTextureID)(intptr_t)tex);
                    t->SetStatus(ImTextureStatus_OK);
                }
            }
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // --- diagnostic (safe to remove later) ---
        ImDrawData* dd = ImGui::GetDrawData();
        int lists = dd->CmdLists.Size, cmds = 0, vtx = 0;
        for (int i = 0; i < lists; i++) {
            cmds += dd->CmdLists[i]->CmdBuffer.Size;
            vtx  += dd->CmdLists[i]->VtxBuffer.Size;
        }
        FLOW_WARN("DrawData lists={} cmds={} vtx={}", lists, cmds, vtx);

        auto& plat = ImGui::GetPlatformIO();
        if (plat.Textures.Size > 0) {
            ImTextureData* atlas = plat.Textures[0];
            FLOW_WARN("atlas {}x{} Status={} TexID={}", atlas->Width, atlas->Height, (int)atlas->Status, (uint64_t)atlas->TexID);
            if (atlas->Width > 0 && atlas->Height > 0) {
                std::vector<unsigned char> buf((size_t)atlas->Width * atlas->Height * 4);
                glBindTexture(GL_TEXTURE_2D, (GLuint)(uint64_t)atlas->TexID);
                glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf.data());
                int nonblack = 0;
                for (size_t i = 0; i < buf.size(); i++) if (buf[i] != 0) nonblack++;
                FLOW_WARN("atlas nonblack pixels={}/{}", nonblack, (int)buf.size());
            }
        }
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
