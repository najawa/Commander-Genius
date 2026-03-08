#pragma once
#ifdef USE_IMGUI

#include <functional>

namespace CImGuiRender
{
    using RenderCallback = std::function<void()>;

    void setCallback(RenderCallback cb);
    void clearCallback();

    // Called from CSDLVideo::transformScreenToDisplay() each frame
    void render();
}

#endif // USE_IMGUI
