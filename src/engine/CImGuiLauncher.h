#pragma once
#ifdef USE_IMGUI

#include "imgui.h"
#include <vector>
#include <string>
#include <memory>

#ifdef DOWNLOADER
#include "gamedownloader.h"
#endif

struct SDL_Texture;
struct ThreadPoolItem;

struct ImGuiGameEntry
{
    std::string name;
    std::string directory;
    int episode = 0;
    bool isDemo = false;
    std::string version;
    SDL_Texture* previewTexture = nullptr;
    int previewWidth = 0;
    int previewHeight = 0;
};

class CImGuiLauncher
{
public:
    CImGuiLauncher();
    ~CImGuiLauncher();

    // Scan for games in the search paths
    void scanForGames();

    // Render the launcher UI -- call every frame
    // Returns true if user selected a game to start
    bool render();

    // Launch the currently selected game via the event manager
    void launchSelectedGame();

    // Get the selected game info after render() returns true
    // Caller must check render() returned true before calling
    const ImGuiGameEntry& getSelectedGame() const { return mGames.at(mSelectedIndex); }

    bool hasSelection() const { return mSelectedIndex >= 0 && mSelectedIndex < static_cast<int>(mGames.size()); }

private:
    void renderGameList();
    void renderPreviewPanel();
    void renderBottomBar();

    // Game scanning helpers
    void scanDirectory(const std::string& path);
    void scanSubDirectories(const std::string& path, int maxDepth);

    // Load preview image for a game entry
    void loadPreview(int index);

    std::vector<ImGuiGameEntry> mGames;
    int mSelectedIndex = -1;
    bool mStartRequested = false;

#ifdef DOWNLOADER
    // Download manager state
    void renderDownloadModal();
    void startCatalogueLoad();
    void startGameDownload(int catalogueIndex);

    std::vector<GameCatalogueEntry> mCatalogueEntries;
    bool mShowDownloadModal = false;
    bool mCatalogueLoaded = false;
    bool mCatalogueLoading = false;
    int mDownloadProgress = 0;
    int mDownloadErrorCode = 0;
    bool mCancelDownload = false;
    bool mDownloading = false;
    int mDownloadingIndex = -1;
    std::shared_ptr<ThreadPoolItem> mDownloadThread;
#endif
};

#endif // USE_IMGUI
