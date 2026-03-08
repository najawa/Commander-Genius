#ifdef USE_IMGUI

#include "CImGuiGameMenu.h"
#include "CImGuiTheme.h"

#include <base/GsEventContainer.h>

#include "engine/core/mode/Scene.h"

void CImGuiGameMenu::open()
{
    mIsOpen = true;
    mCurrentScreen = Screen::MAIN;
}

void CImGuiGameMenu::close()
{
    mIsOpen = false;
    mCurrentScreen = Screen::MAIN;
}

void CImGuiGameMenu::render()
{
    if (!mIsOpen)
        return;

    // Semi-transparent fullscreen overlay behind the menu
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));
    ImGui::Begin("##MenuOverlay", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoInputs);
    ImGui::End();
    ImGui::PopStyleColor();

    switch (mCurrentScreen)
    {
        case Screen::MAIN:     renderMainMenu();          break;
        case Screen::VIDEO:    renderVideoSettings();     break;
        case Screen::AUDIO:    renderAudioSettings();     break;
        case Screen::CONTROLS: renderControlsSettings();  break;
        case Screen::SAVE:     renderSaveGame();          break;
        case Screen::LOAD:     renderLoadGame();          break;
        default:               renderMainMenu();          break;
    }
}

void CImGuiGameMenu::renderMainMenu()
{
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(300, 0));

    ImGui::Begin("##GameMenu", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

    // Centered "PAUSED" title
    ImGui::PushStyleColor(ImGuiCol_Text, CImGuiTheme::accentPrimary());
    const float textWidth = ImGui::CalcTextSize("PAUSED").x;
    ImGui::SetCursorPosX((300.0f - textWidth) / 2.0f);
    ImGui::Text("PAUSED");
    ImGui::PopStyleColor();

    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Continue", ImVec2(-1, 0)))
        close();

    if (ImGui::Button("Save Game", ImVec2(-1, 0)))
        mCurrentScreen = Screen::SAVE;

    if (ImGui::Button("Load Game", ImVec2(-1, 0)))
        mCurrentScreen = Screen::LOAD;

    ImGui::Spacing();

    if (ImGui::Button("Video", ImVec2(-1, 0)))
        mCurrentScreen = Screen::VIDEO;

    if (ImGui::Button("Audio", ImVec2(-1, 0)))
        mCurrentScreen = Screen::AUDIO;

    if (ImGui::Button("Controls", ImVec2(-1, 0)))
        mCurrentScreen = Screen::CONTROLS;

    ImGui::Spacing();

    // Quit to Menu button in accent red
    ImGui::PushStyleColor(ImGuiCol_Button, CImGuiTheme::accentPrimary());
    if (ImGui::Button("Quit to Menu", ImVec2(-1, 0)))
    {
        gEventManager.add(new EventEndGamePlay());
        close();
    }
    ImGui::PopStyleColor();

    ImGui::End();
}

// Stub implementations for settings screens (Tasks 11-14 will fill these in)

void CImGuiGameMenu::renderVideoSettings()
{
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 0));

    ImGui::Begin("##VideoSettings", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushStyleColor(ImGuiCol_Text, CImGuiTheme::accentPrimary());
    ImGui::Text("VIDEO SETTINGS");
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Text("(Coming soon)");
    ImGui::Spacing();

    if (ImGui::Button("Back", ImVec2(-1, 0)))
        mCurrentScreen = Screen::MAIN;

    ImGui::End();
}

void CImGuiGameMenu::renderAudioSettings()
{
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 0));

    ImGui::Begin("##AudioSettings", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushStyleColor(ImGuiCol_Text, CImGuiTheme::accentPrimary());
    ImGui::Text("AUDIO SETTINGS");
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Text("(Coming soon)");
    ImGui::Spacing();

    if (ImGui::Button("Back", ImVec2(-1, 0)))
        mCurrentScreen = Screen::MAIN;

    ImGui::End();
}

void CImGuiGameMenu::renderControlsSettings()
{
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 0));

    ImGui::Begin("##ControlsSettings", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushStyleColor(ImGuiCol_Text, CImGuiTheme::accentPrimary());
    ImGui::Text("CONTROLS");
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Text("(Coming soon)");
    ImGui::Spacing();

    if (ImGui::Button("Back", ImVec2(-1, 0)))
        mCurrentScreen = Screen::MAIN;

    ImGui::End();
}

void CImGuiGameMenu::renderSaveGame()
{
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 0));

    ImGui::Begin("##SaveGame", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushStyleColor(ImGuiCol_Text, CImGuiTheme::accentPrimary());
    ImGui::Text("SAVE GAME");
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Text("(Coming soon)");
    ImGui::Spacing();

    if (ImGui::Button("Back", ImVec2(-1, 0)))
        mCurrentScreen = Screen::MAIN;

    ImGui::End();
}

void CImGuiGameMenu::renderLoadGame()
{
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 0));

    ImGui::Begin("##LoadGame", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushStyleColor(ImGuiCol_Text, CImGuiTheme::accentPrimary());
    ImGui::Text("LOAD GAME");
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Text("(Coming soon)");
    ImGui::Spacing();

    if (ImGui::Button("Back", ImVec2(-1, 0)))
        mCurrentScreen = Screen::MAIN;

    ImGui::End();
}

#endif // USE_IMGUI
