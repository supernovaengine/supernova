// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "Theme.h"

#include "imgui_internal.h"

namespace {

ImGuiStyle g_baseStyle;
bool g_hasBaseStyle = false;
float g_appliedDpiScale = 0.0f;

}

ImVec4 doriax::editor::Theme::Colors::ButtonActivated;
ImVec4 doriax::editor::Theme::Colors::FileCardBackground;
ImVec4 doriax::editor::Theme::Colors::FileCardBackgroundHovered;
ImVec4 doriax::editor::Theme::Colors::SubtleText;
ImVec4 doriax::editor::Theme::Colors::filenameLabel;
ImVec4 doriax::editor::Theme::Colors::ExtEntityButton;
ImVec4 doriax::editor::Theme::Colors::ExtEntityButtonHovered;
ImVec4 doriax::editor::Theme::Colors::ExtEntityButtonActive;
ImVec4 doriax::editor::Theme::Colors::NestedHeader;
ImVec4 doriax::editor::Theme::Colors::NestedHeaderHovered;
ImVec4 doriax::editor::Theme::Colors::NestedHeaderActive;
ImVec4 doriax::editor::Theme::Colors::DisabledGreenText;
ImVec4 doriax::editor::Theme::Colors::ChildSceneText;
ImVec4 doriax::editor::Theme::Colors::SubSelectionText;
ImVec4 doriax::editor::Theme::Colors::WarningText;

void doriax::editor::Theme::apply() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text]                       = ImVec4(0.75f, 0.73f, 0.73f, 1.00f);
    colors[ImGuiCol_TextDisabled]               = ImVec4(0.47f, 0.44f, 0.42f, 1.00f);
    colors[ImGuiCol_WindowBg]                   = ImVec4(0.16f, 0.15f, 0.14f, 1.00f);
    colors[ImGuiCol_ChildBg]                    = ImVec4(0.16f, 0.15f, 0.14f, 1.00f);
    colors[ImGuiCol_PopupBg]                    = ImVec4(0.27f, 0.25f, 0.24f, 1.00f);
    colors[ImGuiCol_Border]                     = ImVec4(0.11f, 0.10f, 0.09f, 1.00f);
    colors[ImGuiCol_BorderShadow]               = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                    = ImVec4(0.05f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]             = ImVec4(0.11f, 0.10f, 0.09f, 1.00f);
    colors[ImGuiCol_FrameBgActive]              = ImVec4(0.11f, 0.10f, 0.09f, 1.00f);
    colors[ImGuiCol_TitleBg]                    = ImVec4(0.16f, 0.15f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBgActive]              = ImVec4(0.27f, 0.25f, 0.24f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]           = ImVec4(0.16f, 0.15f, 0.14f, 1.00f);
    colors[ImGuiCol_MenuBarBg]                  = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ScrollbarBg]                = ImVec4(0.02f, 0.02f, 0.02f, 0.00f);
    colors[ImGuiCol_ScrollbarGrab]              = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]       = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]        = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]                  = ImVec4(0.28f, 0.33f, 0.41f, 1.00f);
    colors[ImGuiCol_SliderGrab]                 = ImVec4(0.28f, 0.33f, 0.41f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]           = ImVec4(0.39f, 0.45f, 0.55f, 1.00f);
    colors[ImGuiCol_Button]                     = ImVec4(0.28f, 0.33f, 0.41f, 1.00f);
    colors[ImGuiCol_ButtonHovered]              = ImVec4(0.34f, 0.40f, 0.48f, 1.00f);
    colors[ImGuiCol_ButtonActive]               = ImVec4(0.39f, 0.45f, 0.55f, 1.00f);
    colors[ImGuiCol_Header]                     = ImVec4(0.27f, 0.25f, 0.24f, 1.00f);
    colors[ImGuiCol_HeaderHovered]              = ImVec4(0.34f, 0.33f, 0.31f, 1.00f);
    colors[ImGuiCol_HeaderActive]               = ImVec4(0.34f, 0.33f, 0.31f, 1.00f);
    colors[ImGuiCol_Separator]                  = ImVec4(0.34f, 0.33f, 0.31f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]           = ImVec4(0.34f, 0.33f, 0.31f, 1.00f);
    colors[ImGuiCol_SeparatorActive]            = ImVec4(0.47f, 0.44f, 0.42f, 1.00f);
    colors[ImGuiCol_ResizeGrip]                 = ImVec4(0.34f, 0.33f, 0.31f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered]          = ImVec4(0.47f, 0.44f, 0.42f, 1.00f);
    colors[ImGuiCol_ResizeGripActive]           = ImVec4(0.47f, 0.44f, 0.42f, 1.00f);
    colors[ImGuiCol_TabHovered]                 = ImVec4(0.34f, 0.33f, 0.31f, 1.00f);
    colors[ImGuiCol_Tab]                        = ImVec4(0.27f, 0.25f, 0.24f, 1.00f);
    colors[ImGuiCol_TabSelected]                = ImVec4(0.34f, 0.33f, 0.31f, 1.00f);
    colors[ImGuiCol_TabSelectedOverline]        = ImVec4(0.47f, 0.44f, 0.42f, 1.00f);
    colors[ImGuiCol_TabDimmed]                  = ImVec4(0.27f, 0.25f, 0.24f, 1.00f);
    colors[ImGuiCol_TabDimmedSelected]          = ImVec4(0.34f, 0.33f, 0.31f, 1.00f);
    colors[ImGuiCol_TabDimmedSelectedOverline]  = ImVec4(0.34f, 0.33f, 0.31f, 1.00f);
    colors[ImGuiCol_DockingPreview]             = ImVec4(0.23f, 0.51f, 0.96f, 0.78f);
    colors[ImGuiCol_DockingEmptyBg]             = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines]                  = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]           = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]              = ImVec4(0.36f, 0.43f, 0.48f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]       = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]              = ImVec4(0.27f, 0.25f, 0.24f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]          = ImVec4(0.47f, 0.44f, 0.42f, 1.00f);
    colors[ImGuiCol_TableBorderLight]           = ImVec4(0.34f, 0.33f, 0.31f, 1.00f);
    colors[ImGuiCol_TableRowBg]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]              = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextLink]                   = ImVec4(0.23f, 0.51f, 0.96f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]             = ImVec4(0.23f, 0.51f, 0.96f, 0.38f);
    colors[ImGuiCol_DragDropTarget]             = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]               = ImVec4(0.23f, 0.51f, 0.96f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]      = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]          = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]           = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[ImGuiCol_CheckboxSelectedBg]         = colors[ImGuiCol_FrameBg];

    Colors::ButtonActivated = ImLerp(colors[ImGuiCol_Button], colors[ImGuiCol_NavHighlight], 0.4f);
    Colors::FileCardBackground = ImLerp(colors[ImGuiCol_WindowBg], colors[ImGuiCol_FrameBg], 0.5f);
    Colors::FileCardBackgroundHovered = ImLerp(colors[ImGuiCol_WindowBg], colors[ImGuiCol_HeaderHovered], 0.25f);
    Colors::SubtleText = ImLerp(colors[ImGuiCol_Text], colors[ImGuiCol_NavHighlight], 0.4f);
    Colors::filenameLabel = ImVec4(50.0f/255.0f, 50.0f/255.0f, 50.0f/255.0f, 1.0f);
    Colors::ExtEntityButton = ImVec4(colors[ImGuiCol_Button].x * 0.85f, colors[ImGuiCol_Button].y * 0.9f, colors[ImGuiCol_Button].z * 1.15f, colors[ImGuiCol_Button].w);
    Colors::ExtEntityButtonHovered = ImVec4(colors[ImGuiCol_ButtonHovered].x * 0.85f, colors[ImGuiCol_ButtonHovered].y * 0.9f, colors[ImGuiCol_ButtonHovered].z * 1.15f, colors[ImGuiCol_ButtonHovered].w);
    Colors::ExtEntityButtonActive = ImVec4(colors[ImGuiCol_ButtonActive].x * 0.85f, colors[ImGuiCol_ButtonActive].y * 0.9f, colors[ImGuiCol_ButtonActive].z * 1.15f, colors[ImGuiCol_ButtonActive].w);
    Colors::NestedHeader = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);
    Colors::NestedHeaderHovered = ImVec4(0.30f, 0.30f, 0.35f, 1.00f);
    Colors::NestedHeaderActive = ImVec4(0.35f, 0.35f, 0.40f, 1.00f);
    Colors::DisabledGreenText = ImLerp(colors[ImGuiCol_TextDisabled],
                                       ImVec4(0.38f, 0.72f, 0.46f, 1.00f),
                                       0.58f);
    Colors::ChildSceneText = ImVec4(0.55f, 0.80f, 0.85f, 1.0f); // Soft teal for child scenes
    Colors::SubSelectionText = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    Colors::WarningText = ImVec4(1.00f, 0.85f, 0.40f, 1.00f);

    // main
    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(10, 4);
    style.ItemSpacing = ImVec2(8, 8);
    style.ItemInnerSpacing = ImVec2(8, 8);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing = 16;
    style.ScrollbarSize = 12;
    style.GrabRounding = 12;

    // borders
    style.WindowBorderSize = 0;
    style.ChildBorderSize = 0;
    style.PopupBorderSize = 0;
    style.FrameBorderSize = 0;
    style.TabBorderSize = 0;
    style.TabBarBorderSize = 1;
    style.TabBarOverlineSize = 2;

    // rounding
    style.WindowRounding = 2;
    style.ChildRounding = 2;
    style.FrameRounding = 2;
    style.PopupRounding = 2;
    style.ScrollbarRounding = 2;
    style.GrabRounding = 2;
    style.TabRounding = 2;

    // tables
    style.CellPadding = ImVec2(8, 4);
    style.TableAngledHeadersAngle = 0.61; //35 deg
    style.TableAngledHeadersTextAlign = ImVec2(0.50, 0.00);

    // widgets
    style.WindowTitleAlign = ImVec2(0.00, 0.50);
    style.WindowMenuButtonPosition = ImGuiDir::ImGuiDir_Right;
    style.ColorButtonPosition = ImGuiDir::ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.50, 0.50);
    style.SelectableTextAlign = ImVec2(0.00, 0.00);
    style.SeparatorTextBorderSize = 1;
    style.SeparatorTextAlign = ImVec2(0.00, 0.50);
    style.SeparatorTextPadding = ImVec2(16, 0);
    style.LogSliderDeadzone = 4;

    // docking
    style.DockingSeparatorSize = 6;

    g_baseStyle = style;
    g_hasBaseStyle = true;
    g_appliedDpiScale = 0.0f;
}

void doriax::editor::Theme::applyDpiScale(float dpiScale) {
    if (!g_hasBaseStyle) {
        return;
    }
    if (dpiScale < 0.5f || dpiScale > 8.0f) {
        dpiScale = 1.0f;
    }
    if (g_appliedDpiScale == dpiScale) {
        return;
    }

    ImGuiStyle& style = ImGui::GetStyle();
    const float fontScaleDpi = style.FontScaleDpi;
    style = g_baseStyle;
    style.ScaleAllSizes(dpiScale);
    // ConfigDpiScaleFonts overwrites FontScaleDpi per window; keep the current
    // value so this reset does not briefly unscale fonts.
    style.FontScaleDpi = fontScaleDpi;
    g_appliedDpiScale = dpiScale;
}

float doriax::editor::Theme::dpiScale() {
    ImGuiContext* ctx = ImGui::GetCurrentContext();
    if (ctx && ctx->CurrentWindow && ctx->CurrentWindow->Viewport && ctx->CurrentWindow->Viewport->DpiScale > 0.0f) {
        return ctx->CurrentWindow->Viewport->DpiScale;
    }
    return (g_appliedDpiScale > 0.0f) ? g_appliedDpiScale : 1.0f;
}

float doriax::editor::Theme::dpi(float value) {
    return value * dpiScale();
}

ImVec2 doriax::editor::Theme::dpi(const ImVec2& value) {
    const float scale = dpiScale();
    return ImVec2(value.x * scale, value.y * scale);
}
