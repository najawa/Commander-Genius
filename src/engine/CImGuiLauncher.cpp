#ifdef USE_IMGUI

#include "CImGuiLauncher.h"
#include "CImGuiTheme.h"
#include "version.h"

static constexpr float BOTTOM_BAR_HEIGHT = 40.0f;
static constexpr float GAME_LIST_WIDTH_RATIO = 0.45f;
static constexpr float PREVIEW_IMAGE_HEIGHT = 200.0f;
static constexpr float START_BUTTON_HEIGHT = 40.0f;
static const ImVec4 BLACK_TEXT(0.0f, 0.0f, 0.0f, 1.0f);

CImGuiLauncher::CImGuiLauncher()
{
    scanForGames();
}

void CImGuiLauncher::scanForGames()
{
    // TODO Task 7: Wire to existing game scanner
    // For now, populate test data
    mGames.clear();
    mGames.push_back({"Commander Keen 1", "games/Keen1", 1, false, "1.33"});
    mGames.push_back({"Commander Keen 4", "games/Keen4", 4, false, "1.4 EGA"});
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

    // Settings button right-aligned
    const float settingsWidth = ImGui::CalcTextSize("Settings").x + ImGui::GetStyle().FramePadding.x * 2;
    ImGui::SameLine(ImGui::GetWindowWidth() - settingsWidth - ImGui::GetStyle().WindowPadding.x);
    if (ImGui::Button("Settings"))
    {
        // TODO: open settings
    }

    ImGui::Separator();

    // Two-column layout
    const float listWidth = ImGui::GetWindowWidth() * GAME_LIST_WIDTH_RATIO;

    ImGui::BeginChild("##GameList", ImVec2(listWidth, -BOTTOM_BAR_HEIGHT), ImGuiChildFlags_Borders);
    renderGameList();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("##Preview", ImVec2(0, -BOTTOM_BAR_HEIGHT), ImGuiChildFlags_Borders);
    renderPreviewPanel();
    ImGui::EndChild();

    // Bottom bar
    renderBottomBar();

    ImGui::End();

    return mStartRequested;
}

void CImGuiLauncher::renderGameList()
{
    const int count = static_cast<int>(mGames.size());
    for (int i = 0; i < count; i++)
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
    const ImVec2 previewSize(ImGui::GetContentRegionAvail().x, PREVIEW_IMAGE_HEIGHT);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, CImGuiTheme::darkBg());
    if (ImGui::BeginChild("##PreviewImg", previewSize, ImGuiChildFlags_Borders))
    {
        ImGui::Text("[ Preview Image ]");
    }
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
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, CImGuiTheme::accentSecondaryHovered());
    ImGui::PushStyleColor(ImGuiCol_Text, BLACK_TEXT);
    if (ImGui::Button("  START GAME  ", ImVec2(-1, START_BUTTON_HEIGHT)))
    {
        mStartRequested = true;
    }
    ImGui::PopStyleColor(3);
}

void CImGuiLauncher::renderBottomBar()
{
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, CImGuiTheme::textDimmed());
    ImGui::Text("Commander Genius %s  |  najawa", CGVERSION);
    const float quitWidth = ImGui::CalcTextSize("Quit").x + ImGui::GetStyle().FramePadding.x * 2;
    ImGui::SameLine(ImGui::GetWindowWidth() - quitWidth - ImGui::GetStyle().WindowPadding.x);
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Button, CImGuiTheme::accentPrimary());
    if (ImGui::SmallButton("Quit"))
    {
        // TODO: emit GMQuit event
    }
    ImGui::PopStyleColor();
}

#endif // USE_IMGUI
