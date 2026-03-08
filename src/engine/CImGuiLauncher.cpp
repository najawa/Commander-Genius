#ifdef USE_IMGUI

#include "CImGuiLauncher.h"
#include "CImGuiTheme.h"
#include <base/GsLogging.h>

CImGuiLauncher::CImGuiLauncher()
{
    scanForGames();
}

void CImGuiLauncher::scanForGames()
{
    // TODO Task 7: Wire to existing game scanner
    // For now, populate test data
    mGames.clear();

    ImGuiGameEntry test;
    test.name = "Commander Keen 1";
    test.directory = "games/Keen1";
    test.episode = 1;
    test.version = "1.33";
    mGames.push_back(test);

    test.name = "Commander Keen 4";
    test.directory = "games/Keen4";
    test.episode = 4;
    test.version = "1.4 EGA";
    mGames.push_back(test);
}

bool CImGuiLauncher::render()
{
    mStartRequested = false;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("##Launcher", nullptr, flags);

    // Title
    ImGui::PushStyleColor(ImGuiCol_Text, CImGuiTheme::accentPrimary());
    ImGui::Text("COMMANDER GENIUS");
    ImGui::PopStyleColor();

    ImGui::SameLine(ImGui::GetWindowWidth() - 100);
    if (ImGui::Button("Settings"))
    {
        // TODO: open settings
    }

    ImGui::Separator();

    // Two-column layout
    const float listWidth = ImGui::GetWindowWidth() * 0.45f;

    ImGui::BeginChild("##GameList", ImVec2(listWidth, -40), true);
    renderGameList();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("##Preview", ImVec2(0, -40), true);
    renderPreviewPanel();
    ImGui::EndChild();

    // Bottom bar
    renderBottomBar();

    ImGui::End();

    return mStartRequested;
}

void CImGuiLauncher::renderGameList()
{
    for (int i = 0; i < static_cast<int>(mGames.size()); i++)
    {
        const bool isSelected = (mSelectedIndex == i);

        if (isSelected)
        {
            ImGui::PushStyleColor(ImGuiCol_Header, CImGuiTheme::accentPrimary());
        }

        if (ImGui::Selectable(mGames[i].name.c_str(), isSelected,
                              ImGuiSelectableFlags_AllowDoubleClick))
        {
            mSelectedIndex = i;
            if (ImGui::IsMouseDoubleClicked(0))
            {
                mStartRequested = true;
            }
        }

        if (isSelected)
        {
            ImGui::PopStyleColor();
        }
    }
}

void CImGuiLauncher::renderPreviewPanel()
{
    if (mSelectedIndex < 0 || mSelectedIndex >= static_cast<int>(mGames.size()))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, CImGuiTheme::textDimmed());
        ImGui::Text("Select a game to begin");
        ImGui::PopStyleColor();
        return;
    }

    const auto& game = mGames[mSelectedIndex];

    // Preview image placeholder
    ImVec2 previewSize(ImGui::GetContentRegionAvail().x, 200);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.07f, 0.12f, 1.0f));
    ImGui::BeginChild("##PreviewImg", previewSize, true);
    ImGui::Text("[ Preview Image ]");
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::Spacing();

    // Game info
    ImGui::Text("Episode: %d", game.episode);
    ImGui::Text("Version: %s", game.version.c_str());
    ImGui::Text("Path: %s", game.directory.c_str());
    if (game.isDemo)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, CImGuiTheme::accentPrimary());
        ImGui::Text("DEMO VERSION");
        ImGui::PopStyleColor();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // Start button
    ImGui::PushStyleColor(ImGuiCol_Button, CImGuiTheme::accentSecondary());
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.38f, 0.92f, 0.48f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    if (ImGui::Button("  START GAME  ", ImVec2(-1, 40)))
    {
        mStartRequested = true;
    }
    ImGui::PopStyleColor(3);
}

void CImGuiLauncher::renderBottomBar()
{
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, CImGuiTheme::textDimmed());
    ImGui::Text("Commander Genius v3.4  |  najawa");
    ImGui::SameLine(ImGui::GetWindowWidth() - 60);
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Button, CImGuiTheme::accentPrimary());
    if (ImGui::SmallButton("Quit"))
    {
        // TODO: emit GMQuit event
    }
    ImGui::PopStyleColor();
}

#endif // USE_IMGUI
