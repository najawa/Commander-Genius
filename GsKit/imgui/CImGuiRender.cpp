#ifdef USE_IMGUI

#include "CImGuiRender.h"

namespace CImGuiRender
{
    static RenderCallback sCallback;

    void setCallback(RenderCallback cb)
    {
        sCallback = std::move(cb);
    }

    void clearCallback()
    {
        sCallback = nullptr;
    }

    void render()
    {
        if (sCallback)
        {
            sCallback();
        }
    }
}

#endif // USE_IMGUI
