#ifdef USE_IMGUI

#include "CImGuiGameMenu.h"
#include "CImGuiTheme.h"

#include <base/GsEventContainer.h>
#include <base/video/CVideoDriver.h>
#include <base/audio/Audio.h>
#include <base/CInput.h>

#include "engine/core/mode/Scene.h"
#include "engine/core/CSettings.h"

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

bool CImGuiGameMenu::beginSubScreen(const char* id, const char* title, float width)
{
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(width, 0));

    ImGui::Begin(id, nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushStyleColor(ImGuiCol_Text, CImGuiTheme::accentPrimary());
    const float titleWidth = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPosX((width - titleWidth) / 2.0f);
    ImGui::Text("%s", title);
    ImGui::PopStyleColor();

    ImGui::Separator();
    ImGui::Spacing();

    return true;
}

void CImGuiGameMenu::endSubScreen()
{
    ImGui::Spacing();
    if (ImGui::Button("Back", ImVec2(-1, 0)))
        mCurrentScreen = Screen::MAIN;

    ImGui::End();
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

// ---------------------------------------------------------------------------
// Task 11: Video Settings
// ---------------------------------------------------------------------------

void CImGuiGameMenu::renderVideoSettings()
{
    beginSubScreen("##VideoSettings", "VIDEO SETTINGS");

    CVidConfig vidConf = gVideoDriver.getVidConfig();

    bool fullscreen = vidConf.mFullscreen;
    if (ImGui::Checkbox("Fullscreen", &fullscreen))
    {
        const CVidConfig oldConf = gVideoDriver.getVidConfig();
        CVidConfig newConf = oldConf;
        newConf.mFullscreen = fullscreen;
        gVideoDriver.setVidConfig(newConf);
        if (!gVideoDriver.applyMode())
            gVideoDriver.setVidConfig(oldConf);
        else
            gVideoDriver.start();
    }

    bool vsync = vidConf.mVSync;
    if (ImGui::Checkbox("VSync", &vsync))
    {
        const CVidConfig oldConf = gVideoDriver.getVidConfig();
        CVidConfig newConf = oldConf;
        newConf.mVSync = vsync;
        gVideoDriver.setVidConfig(newConf);
        if (!gVideoDriver.applyMode())
            gVideoDriver.setVidConfig(oldConf);
        else
            gVideoDriver.start();
    }

    bool integerScaling = vidConf.mIntegerScaling;
    if (ImGui::Checkbox("Integer Scaling", &integerScaling))
    {
        const CVidConfig oldConf = gVideoDriver.getVidConfig();
        CVidConfig newConf = oldConf;
        newConf.mIntegerScaling = integerScaling;
        gVideoDriver.setVidConfig(newConf);
        if (!gVideoDriver.applyMode())
            gVideoDriver.setVidConfig(oldConf);
        else
            gVideoDriver.start();
    }

    const char* qualityItems[] = { "Nearest", "Linear" };
    int currentQuality = (vidConf.mRenderScQuality == CVidConfig::RenderQuality::LINEAR) ? 1 : 0;
    if (ImGui::Combo("Render Quality", &currentQuality, qualityItems, 2))
    {
        const CVidConfig oldConf = gVideoDriver.getVidConfig();
        CVidConfig newConf = oldConf;
        newConf.mRenderScQuality = (currentQuality == 1)
            ? CVidConfig::RenderQuality::LINEAR
            : CVidConfig::RenderQuality::NEAREST;
        gVideoDriver.setVidConfig(newConf);
        if (!gVideoDriver.applyMode())
            gVideoDriver.setVidConfig(oldConf);
        else
            gVideoDriver.start();
    }

    endSubScreen();
}

// ---------------------------------------------------------------------------
// Task 12: Audio Settings
// ---------------------------------------------------------------------------

void CImGuiGameMenu::renderAudioSettings()
{
    beginSubScreen("##AudioSettings", "AUDIO SETTINGS");

    int soundVol = static_cast<int>(gAudio.getSoundVolume());
    if (ImGui::SliderInt("Sound Volume", &soundVol, 0, SDL_MIX_MAXVOLUME))
    {
        gAudio.setSoundVolume(static_cast<Uint8>(soundVol));
    }

    int musicVol = static_cast<int>(gAudio.getMusicVolume());
    if (ImGui::SliderInt("Music Volume", &musicVol, 0, SDL_MIX_MAXVOLUME))
    {
        gAudio.setMusicVolume(static_cast<Uint8>(musicVol));
    }

    ImGui::Spacing();

    int oplAmp = gAudio.getOplAmp();
    if (ImGui::SliderInt("OPL Amp", &oplAmp, 100, 400))
    {
        gAudio.setOplAmp(oplAmp);
    }

    int pcSpkVol = gAudio.getPCSpeakerVol();
    if (ImGui::SliderInt("PC Speaker Vol", &pcSpkVol, 0, 100))
    {
        gAudio.setPcSpeakerVol(pcSpkVol);
    }

    endSubScreen();
}

// ---------------------------------------------------------------------------
// Task 13: Controls Settings (read-only key binding table)
// ---------------------------------------------------------------------------

void CImGuiGameMenu::renderControlsSettings()
{
    beginSubScreen("##ControlsSettings", "CONTROLS", 450.0f);

    struct ControlEntry
    {
        const char* label;
        InpCmd command;
    };

    static constexpr ControlEntry kEntries[] = {
        { "Left",       IC_LEFT      },
        { "Right",      IC_RIGHT     },
        { "Up",         IC_UP        },
        { "Down",       IC_DOWN      },
        { "Jump",       IC_JUMP      },
        { "Pogo",       IC_POGO      },
        { "Fire",       IC_FIRE      },
        { "Run",        IC_RUN       },
        { "Status",     IC_STATUS    },
        { "Cam/Revive", IC_CAMLEAD   },
        { "Help",       IC_HELP      },
        { "Back",       IC_BACK      },
        { "Quicksave",  IC_QUICKSAVE },
        { "Quickload",  IC_QUICKLOAD },
    };

    if (ImGui::BeginTable("##KeyBindings", 2,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (const auto& entry : kEntries)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(entry.label);
            ImGui::TableSetColumnIndex(1);
            const std::string keyName =
                gInput.getEventShortName(entry.command, 0);
            ImGui::TextUnformatted(keyName.c_str());
        }

        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, CImGuiTheme::textDimmed());
    ImGui::TextWrapped("Key rebinding coming in a future update.");
    ImGui::PopStyleColor();

    endSubScreen();
}

void CImGuiGameMenu::renderSaveGame()
{
    beginSubScreen("##SaveGame", "SAVE GAME");
    ImGui::Text("(Coming soon)");
    endSubScreen();
}

void CImGuiGameMenu::renderLoadGame()
{
    beginSubScreen("##LoadGame", "LOAD GAME");
    ImGui::Text("(Coming soon)");
    endSubScreen();
}

#endif // USE_IMGUI
