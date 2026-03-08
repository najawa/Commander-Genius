#pragma once
#ifdef USE_IMGUI

#include "imgui.h"
#include <string>

class CImGuiGameMenu
{
public:
    enum class Screen { MAIN, VIDEO, AUDIO, CONTROLS, GAME_OPTIONS, SAVE, LOAD };

    void open();
    void close();
    bool isOpen() const { return mIsOpen; }

    // Call every frame -- renders nothing if closed
    void render();

private:
    void renderMainMenu();
    void renderVideoSettings();
    void renderAudioSettings();
    void renderControlsSettings();
    void renderSaveGame();
    void renderLoadGame();

    // Helper: renders a centered sub-screen window header and returns true if window is open
    bool beginSubScreen(const char* id, const char* title, float width = 400.0f);
    void endSubScreen();

    bool mIsOpen = false;
    Screen mCurrentScreen = Screen::MAIN;
};

#endif // USE_IMGUI
