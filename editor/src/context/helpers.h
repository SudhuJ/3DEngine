#pragma once

#include "../../engine/include/flow.h"

#include <vendors/FA.h>
#include <vendors/imgui/imgui.h>
#include <vendors/imgui/imgui_internal.h>

#include <vendors/imgui/backends/imgui_impl_glfw.h>
#include <vendors/imgui/backends/imgui_impl_opengl3.h>

// #include <vendors/imgui/backends/imgui_impl_opengl3_loader.h>


using namespace flow;

#define FONT_FILE "resources/fonts/Roboto-Medium.ttf"
#define ICON_FONT "resources/fonts/fa-solid-900.ttf"
#define SHADER_VERSION "#version 130"

#define REGULAR_FONT_SIZE 17
#define SMALL_FONT_SIZE 15

#define LABEL_SPACING 130
#define MENUBAR_HEIHT 50

#define ASSET_SPACING 10
#define ASSET_SIZE 90

// helper macros
#define LINE_HEIGHT() ImGui::GetTextLineHeight()
#define USE_SMALL_FONT() ImGui::SetCurrentFont(ImGui::GetIO().Fonts->Fonts[1], SMALL_FONT_SIZE, 0.0f)
#define USE_REGULAR_FONT() ImGui::SetCurrentFont(ImGui::GetIO().Fonts->Fonts[0], REGULAR_FONT_SIZE, 0.0f)
#define TEXT_HEIGHT() ImGui::GetTextLineHeight() + (2.0f * ImGui::GetStyle().FramePadding.y)
