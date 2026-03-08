#pragma once
#ifdef USE_IMGUI

#include "imgui.h"
#include <vector>
#include <string>

struct ImGuiGameEntry
{
    std::string name;
    std::string directory;
    int episode = 0;
    bool isDemo = false;
    std::string version;
    // Preview texture loaded later
};

class CImGuiLauncher
{
public:
    CImGuiLauncher();

    // Scan for games in the search paths
    void scanForGames();

    // Render the launcher UI -- call every frame
    // Returns true if user selected a game to start
    bool render();

    // Get the selected game info after render() returns true
    // Caller must check render() returned true before calling
    const ImGuiGameEntry& getSelectedGame() const { return mGames.at(mSelectedIndex); }

    bool hasSelection() const { return mSelectedIndex >= 0 && mSelectedIndex < static_cast<int>(mGames.size()); }

private:
    void renderGameList();
    void renderPreviewPanel();
    void renderBottomBar();

    std::vector<ImGuiGameEntry> mGames;
    int mSelectedIndex = -1;
    bool mStartRequested = false;
};

#endif // USE_IMGUI
