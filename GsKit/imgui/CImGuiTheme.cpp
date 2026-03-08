#ifdef USE_IMGUI

#include "CImGuiTheme.h"

namespace CImGuiTheme
{

static ImVec4 hexToImVec4(uint32_t hex, float alpha = 1.0f)
{
    return ImVec4(
        ((hex >> 16) & 0xFF) / 255.0f,
        ((hex >> 8) & 0xFF) / 255.0f,
        (hex & 0xFF) / 255.0f,
        alpha
    );
}

ImVec4 background()      { return hexToImVec4(0x1a1a2e); }
ImVec4 panel()           { return hexToImVec4(0x16213e); }
ImVec4 accentPrimary()   { return hexToImVec4(0xe94560); }
ImVec4 accentSecondary() { return hexToImVec4(0x53d769); }
ImVec4 text()            { return hexToImVec4(0xeaeaea); }
ImVec4 textDimmed()      { return hexToImVec4(0x8a8a8a); }

void apply()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.PopupRounding = 0.0f;

    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;

    style.WindowPadding = ImVec2(12, 12);
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(8, 6);
    style.ScrollbarSize = 10.0f;

    ImVec4* colors = style.Colors;

    colors[ImGuiCol_WindowBg]           = panel();
    colors[ImGuiCol_ChildBg]            = hexToImVec4(0x0f1525);
    colors[ImGuiCol_PopupBg]            = panel();
    colors[ImGuiCol_Border]             = hexToImVec4(0x3a4a6e);
    colors[ImGuiCol_Text]               = text();
    colors[ImGuiCol_TextDisabled]       = textDimmed();
    colors[ImGuiCol_FrameBg]            = hexToImVec4(0x0f1525);
    colors[ImGuiCol_FrameBgHovered]     = hexToImVec4(0x1e2d4d);
    colors[ImGuiCol_FrameBgActive]      = hexToImVec4(0x2a3d5e);
    colors[ImGuiCol_TitleBg]            = background();
    colors[ImGuiCol_TitleBgActive]      = hexToImVec4(0x1e1e3a);
    colors[ImGuiCol_TitleBgCollapsed]   = background();
    colors[ImGuiCol_Button]             = hexToImVec4(0x1e2d4d);
    colors[ImGuiCol_ButtonHovered]      = accentPrimary();
    colors[ImGuiCol_ButtonActive]       = hexToImVec4(0xc73a52);
    colors[ImGuiCol_Header]             = hexToImVec4(0x1e2d4d);
    colors[ImGuiCol_HeaderHovered]      = accentPrimary();
    colors[ImGuiCol_HeaderActive]       = hexToImVec4(0xc73a52);
    colors[ImGuiCol_ScrollbarBg]        = hexToImVec4(0x0f1525);
    colors[ImGuiCol_ScrollbarGrab]      = hexToImVec4(0x3a4a6e);
    colors[ImGuiCol_ScrollbarGrabHovered] = accentPrimary();
    colors[ImGuiCol_ScrollbarGrabActive]  = hexToImVec4(0xc73a52);
    colors[ImGuiCol_CheckMark]          = accentSecondary();
    colors[ImGuiCol_SliderGrab]         = accentPrimary();
    colors[ImGuiCol_SliderGrabActive]   = hexToImVec4(0xc73a52);
    colors[ImGuiCol_Separator]          = hexToImVec4(0x3a4a6e);
    colors[ImGuiCol_SeparatorHovered]   = accentPrimary();
    colors[ImGuiCol_SeparatorActive]    = accentPrimary();
    colors[ImGuiCol_Tab]                = hexToImVec4(0x1e2d4d);
    colors[ImGuiCol_TabHovered]         = accentPrimary();
    colors[ImGuiCol_TabSelected]        = hexToImVec4(0x2a3d5e);
    colors[ImGuiCol_NavHighlight]       = accentPrimary();
}

bool loadFont(const std::string& fontPath, float sizePixels)
{
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig config;
    config.PixelSnapH = true;
    config.OversampleH = 1;
    config.OversampleV = 1;

    ImFont* font = io.Fonts->AddFontFromFileTTF(
        fontPath.c_str(), sizePixels, &config
    );

    if (!font)
    {
        io.Fonts->AddFontDefault();
        return false;
    }

    return true;
}

}

#endif
