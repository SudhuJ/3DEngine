#pragma once
#include "helpers.h"

FLOW_INLINE void ThemeMetrics() {
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding    = 8.0f;
    style.ChildRounding     = 8.0f;
    style.PopupRounding     = 8.0f;
    style.FrameRounding     = 4.0f;
    style.TabRounding       = 6.0f;
    style.GrabRounding      = 4.0f;
    style.ScrollbarRounding = 8.0f;

    style.WindowBorderSize  = 1.0f;
    style.ChildBorderSize   = 1.0f;
    style.PopupBorderSize   = 1.0f;
    style.FrameBorderSize   = 1.0f;
    style.TabBorderSize     = 0.0f;

    style.WindowPadding    = ImVec2(10, 10);
    style.FramePadding     = ImVec2(8, 4);
    style.ItemSpacing      = ImVec2(8, 4);
    style.ItemInnerSpacing = ImVec2(6, 6);
    style.CellPadding      = ImVec2(6, 4);
    style.ScrollbarSize    = 12.0f;
    style.GrabMinSize      = 8.0f;

    style.WindowTitleAlign         = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_None;
}

FLOW_INLINE void ApplyDarkTheme() {
    ThemeMetrics();
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* c = style.Colors;
    c[ImGuiCol_Text]                  = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
    c[ImGuiCol_TextDisabled]          = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    c[ImGuiCol_WindowBg]              = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
    c[ImGuiCol_ChildBg]               = ImVec4(0.10f, 0.10f, 0.10f, 0.00f);
    c[ImGuiCol_PopupBg]               = ImVec4(0.11f, 0.11f, 0.11f, 0.98f);
    c[ImGuiCol_Border]                = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    c[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    c[ImGuiCol_FrameBg]               = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    c[ImGuiCol_FrameBgHovered]        = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    c[ImGuiCol_FrameBgActive]         = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    c[ImGuiCol_TitleBg]               = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
    c[ImGuiCol_TitleBgActive]         = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    c[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
    c[ImGuiCol_MenuBarBg]             = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
    c[ImGuiCol_ScrollbarBg]           = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    c[ImGuiCol_ScrollbarGrab]         = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    c[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    c[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    c[ImGuiCol_CheckMark]             = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    c[ImGuiCol_SliderGrab]            = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    c[ImGuiCol_SliderGrabActive]      = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    c[ImGuiCol_Button]                = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    c[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    c[ImGuiCol_ButtonActive]          = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
    c[ImGuiCol_Header]                = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    c[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    c[ImGuiCol_HeaderActive]          = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
    c[ImGuiCol_Separator]             = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    c[ImGuiCol_SeparatorHovered]      = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    c[ImGuiCol_SeparatorActive]       = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    c[ImGuiCol_ResizeGrip]            = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    c[ImGuiCol_ResizeGripHovered]     = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    c[ImGuiCol_ResizeGripActive]      = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    c[ImGuiCol_Tab]                   = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    c[ImGuiCol_TabHovered]            = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    c[ImGuiCol_TabSelected]           = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    c[ImGuiCol_TabDimmed]             = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    c[ImGuiCol_TabDimmedSelected]     = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    c[ImGuiCol_DockingPreview]        = ImVec4(0.60f, 0.60f, 0.60f, 0.40f);
    c[ImGuiCol_DockingEmptyBg]        = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    c[ImGuiCol_PlotLines]             = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    c[ImGuiCol_PlotHistogram]         = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    c[ImGuiCol_TextSelectedBg]        = ImVec4(0.45f, 0.45f, 0.45f, 0.40f);
    c[ImGuiCol_DragDropTarget]        = ImVec4(0.70f, 0.70f, 0.70f, 0.40f);
    c[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.00f, 0.00f, 0.00f, 0.55f);
}

FLOW_INLINE void ApplyLightTheme() {
    ThemeMetrics();
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* c = style.Colors;
    c[ImGuiCol_Text]                  = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    c[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    c[ImGuiCol_WindowBg]              = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
    c[ImGuiCol_ChildBg]               = ImVec4(0.90f, 0.90f, 0.90f, 0.00f);
    c[ImGuiCol_PopupBg]               = ImVec4(0.94f, 0.94f, 0.94f, 0.98f);
    c[ImGuiCol_Border]                = ImVec4(0.72f, 0.72f, 0.72f, 1.00f);
    c[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    c[ImGuiCol_FrameBg]               = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    c[ImGuiCol_FrameBgHovered]        = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    c[ImGuiCol_FrameBgActive]         = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    c[ImGuiCol_TitleBg]               = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    c[ImGuiCol_TitleBgActive]         = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
    c[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    c[ImGuiCol_MenuBarBg]             = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    c[ImGuiCol_ScrollbarBg]           = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    c[ImGuiCol_ScrollbarGrab]         = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    c[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    c[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    c[ImGuiCol_CheckMark]             = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    c[ImGuiCol_SliderGrab]            = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
    c[ImGuiCol_SliderGrabActive]      = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    c[ImGuiCol_Button]                = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    c[ImGuiCol_ButtonHovered]         = ImVec4(0.72f, 0.72f, 0.72f, 1.00f);
    c[ImGuiCol_ButtonActive]          = ImVec4(0.62f, 0.62f, 0.62f, 1.00f);
    c[ImGuiCol_Header]                = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    c[ImGuiCol_HeaderHovered]         = ImVec4(0.72f, 0.72f, 0.72f, 1.00f);
    c[ImGuiCol_HeaderActive]          = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
    c[ImGuiCol_Separator]             = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    c[ImGuiCol_SeparatorHovered]      = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
    c[ImGuiCol_SeparatorActive]       = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    c[ImGuiCol_ResizeGrip]            = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    c[ImGuiCol_ResizeGripHovered]     = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    c[ImGuiCol_ResizeGripActive]      = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    c[ImGuiCol_Tab]                   = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
    c[ImGuiCol_TabHovered]            = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
    c[ImGuiCol_TabSelected]           = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    c[ImGuiCol_TabDimmed]             = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
    c[ImGuiCol_TabDimmedSelected]     = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
    c[ImGuiCol_DockingPreview]        = ImVec4(0.30f, 0.30f, 0.30f, 0.40f);
    c[ImGuiCol_DockingEmptyBg]        = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    c[ImGuiCol_PlotLines]             = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    c[ImGuiCol_PlotHistogram]         = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    c[ImGuiCol_TextSelectedBg]        = ImVec4(0.50f, 0.50f, 0.50f, 0.35f);
    c[ImGuiCol_DragDropTarget]        = ImVec4(0.35f, 0.35f, 0.35f, 0.40f);
    c[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.20f, 0.20f, 0.20f, 0.60f);
}
