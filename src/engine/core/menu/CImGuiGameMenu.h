#pragma once
#ifdef USE_IMGUI

#include "imgui.h"
#include <array>
#include <string>

class CImGuiGameMenu
{
public:
    enum class Screen { MAIN, VIDEO, AUDIO, CONTROLS, SAVE, LOAD };

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

    // Helper: renders a centered sub-screen window header
    bool beginSubScreen(const char* id, const char* title, float width = 400.0f);
    void endSubScreen();

    bool mIsOpen = false;
    Screen mCurrentScreen = Screen::MAIN;

    // Save game state
    int mSelectedSaveSlot = -1;
    std::array<char, 64> mSaveNameBuf = {};

    // Load game state
    int mSelectedLoadSlot = -1;
};

#endif // USE_IMGUI
