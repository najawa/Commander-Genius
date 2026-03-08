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

    bool mIsOpen = false;
    Screen mCurrentScreen = Screen::MAIN;
};

#endif // USE_IMGUI
