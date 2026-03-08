#ifdef USE_IMGUI

#include "CImGuiLauncher.h"
#include "CImGuiTheme.h"
#include "version.h"

#include <base/GsLogging.h>
#include <base/GsEvent.h>
#include <base/GsEventContainer.h>
#include <base/interface/FindFile.h>
#include <base/interface/StringUtils.h>

#include "fileio/CExeFile.h"
#include "fileio/KeenFiles.h"

#include "keen/vorticon/VorticonEngine.h"
#include "keen/galaxy/GalaxyEngine.h"
#ifdef BUILD_REFKEEN
#include "keen/dreams/dreamsengine.h"
#endif // BUILD_REFKEEN

#include <set>

static constexpr float BOTTOM_BAR_HEIGHT = 40.0f;
static constexpr float GAME_LIST_WIDTH_RATIO = 0.45f;
static constexpr float PREVIEW_IMAGE_HEIGHT = 200.0f;
static constexpr float START_BUTTON_HEIGHT = 40.0f;
static const ImVec4 BLACK_TEXT(0.0f, 0.0f, 0.0f, 1.0f);

// ---- Local game-detection helpers (mirrored from CGameLauncher.cpp) ----

namespace
{

struct ExecutableListFiller
{
    std::set<std::string> list;

    bool operator()(const std::string& filename)
    {
        const std::string ext = GetFileExtension(filename);
        if (stringcaseequal(ext, "exe") ||
            stringcaseequal(ext, "lua"))
        {
            list.insert(stringtolower(filename));
        }
        return true;
    }
};

bool detectKeenGame(const std::string& curFname, int& ep, bool& isLua)
{
    ep = -1;
    isLua = false;

#ifdef BUILD_REFKEEN
    // Episode 7 stands for Keen Dreams
    if (curFname.find("kdreams") != std::string::npos)
    {
        ep = 7;
    }
#endif // BUILD_REFKEEN

    for (int i = 1; i <= 6; ++i)
    {
        std::string keenName = "keen" + itoa(i) + "e.exe";
        if (curFname.find(keenName) != std::string::npos)
        {
            ep = i;
            break;
        }

        keenName = "keen" + itoa(i) + ".exe";
        if (curFname.find(keenName) != std::string::npos)
        {
            ep = i;
            break;
        }

        keenName = "keen" + itoa(i) + ".lua";
        if (curFname.find(keenName) != std::string::npos)
        {
            ep = i;
            isLua = true;
            break;
        }

        keenName = "k" + itoa(i) + "demo.exe";
        if (curFname.find(keenName) != std::string::npos)
        {
            ep = i;
            break;
        }
    }

    return ep >= 0;
}

// Extract the last path component for use as a display name
std::string filterGameName(const std::string& path)
{
    std::string text = path;
    size_t pos = 0;

    for (;;)
    {
        pos = text.find('\\');
        if (pos == std::string::npos)
        {
            pos = text.find('/');
            if (pos == std::string::npos)
            {
                break;
            }
        }
        text = text.substr(pos + 1);
    }

    return text;
}

} // anonymous namespace

// ---- CImGuiLauncher implementation ----

CImGuiLauncher::CImGuiLauncher()
{
    scanForGames();
}

void CImGuiLauncher::scanForGames()
{
    mGames.clear();

    gLogging.ftextOut("ImGui Launcher: Game Autodetection Started<br>");

    // Scan root directory for executables
    scanDirectory(".");

    // Recursively scan the games directory (depth 2)
    scanSubDirectories("games", 2);

    if (mGames.empty())
    {
        gLogging.ftextOut("ImGui Launcher: No games detected<br>");
    }
    else
    {
        gLogging.ftextOut("ImGui Launcher: Found %d game(s)<br>",
                          static_cast<int>(mGames.size()));
    }
}

void CImGuiLauncher::scanDirectory(const std::string& path)
{
    gLogging.ftextOut("ImGui Launcher: Scanning %s<br>", path.c_str());

    ExecutableListFiller execList;
    FindFiles(execList, path, false, FM_REG);

    for (const auto& curFname : execList.list)
    {
        int ep = -1;
        bool isLua = false;

        if (!detectKeenGame(curFname, ep, isLua))
        {
            continue;
        }

        gLogging.ftextOut("ImGui Launcher: Keen game detected: %s (ep %d)<br>",
                          curFname.c_str(), ep);

        CExeFile executable;

        if (isLua)
        {
            if (!executable.readMainLuaScript(
                    static_cast<unsigned int>(ep), path))
            {
                continue;
            }
        }
        else
        {
            if (!executable.readKeenExeData(
                    static_cast<unsigned int>(ep), path))
            {
                continue;
            }
        }

        // Build the version string
        const int ver = executable.getEXEVersion();
        std::string verStr;
        if (ver <= 0)
        {
            verStr = "unknown";
        }
        else
        {
            verStr = "v" + itoa(ver / 100) + "." + itoa(ver % 100);
        }

        // Build a display name from the directory
        std::string displayName = filterGameName(path);
        if (displayName.empty())
        {
            displayName = executable.getFileName();
        }

        ImGuiGameEntry entry;
        entry.name      = displayName;
        entry.directory  = path;
        entry.episode    = ep;
        entry.isDemo     = executable.isDemo();
        entry.version    = verStr;

        mGames.push_back(entry);
    }
}

void CImGuiLauncher::scanSubDirectories(const std::string& path,
                                         int maxDepth)
{
    std::set<std::string> dirs;
    FileListAdder fileListAdder;
    GetFileList(dirs, fileListAdder, path, false, FM_DIR);

    for (const auto& subdir : dirs)
    {
        const std::string newPath = JoinPaths(path, subdir);

        scanDirectory(newPath);

        if (maxDepth > 1)
        {
            scanSubDirectories(newPath, maxDepth - 1);
        }
    }
}

void CImGuiLauncher::launchSelectedGame()
{
    if (!hasSelection())
    {
        return;
    }

    const auto& game = mGames[mSelectedIndex];
    const int episode = game.episode;
    const std::string& dir = game.directory;

    if (episode >= 1 && episode <= 3)
    {
        gEventManager.add(new StartVorticonEngine(false, episode, dir));
    }
    else if (episode >= 4 && episode <= 6)
    {
        gEventManager.add(new StartGalaxyEngine(false, episode, dir));
    }
#ifdef BUILD_REFKEEN
    else if (episode == 7)
    {
        gEventManager.add(new StartDreamsEngine(false, dir));
    }
#endif // BUILD_REFKEEN
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

    if (mStartRequested && hasSelection())
    {
        launchSelectedGame();
    }

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
