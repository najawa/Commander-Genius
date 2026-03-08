#pragma once
#ifdef USE_IMGUI

#include "imgui.h"
#include <string>

namespace CImGuiTheme
{
    void apply();
    bool loadFont(const std::string& fontPath, float sizePixels);

    ImVec4 background();
    ImVec4 panel();
    ImVec4 accentPrimary();
    ImVec4 accentSecondary();
    ImVec4 accentSecondaryHovered();
    ImVec4 darkBg();
    ImVec4 text();
    ImVec4 textDimmed();
}

#endif
