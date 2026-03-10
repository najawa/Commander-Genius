# Dear ImGui UI Overhaul — Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Replace the Commander Genius GsKit widget/menu system with Dear ImGui, achieving a polished retro aesthetic across the launcher and all in-game menus.

**Architecture:** Dear ImGui is vendored into `GsKit/imgui/` and initialized alongside SDL2. It renders as an overlay on the existing game frame via `imgui_impl_sdl2` + `imgui_impl_sdlrenderer2` backends. A `USE_IMGUI` CMake flag allows coexistence with the old widget system during migration. ImGui menus emit the same C++ events (`StartVorticonEngine`, `StartGalaxyEngine`, etc.) that the old system uses.

**Tech Stack:** C++17, SDL2, Dear ImGui (v1.91+), Press Start 2P font (OFL)

---

## Phase A: Integrate Dear ImGui

### Task 1: Vendor Dear ImGui source files

**Files:**
- Create: `GsKit/imgui/` directory
- Create: `GsKit/imgui/CMakeLists.txt`

**Step 1: Download Dear ImGui release**

Run from project root:
```bash
mkdir -p GsKit/imgui
curl -L https://github.com/ocornut/imgui/archive/refs/tags/v1.91.8.tar.gz | tar xz --strip-components=1 -C /tmp/imgui-extract
cp /tmp/imgui-extract/imgui.cpp GsKit/imgui/
cp /tmp/imgui-extract/imgui.h GsKit/imgui/
cp /tmp/imgui-extract/imgui_draw.cpp GsKit/imgui/
cp /tmp/imgui-extract/imgui_tables.cpp GsKit/imgui/
cp /tmp/imgui-extract/imgui_widgets.cpp GsKit/imgui/
cp /tmp/imgui-extract/imgui_demo.cpp GsKit/imgui/
cp /tmp/imgui-extract/imgui_internal.h GsKit/imgui/
cp /tmp/imgui-extract/imconfig.h GsKit/imgui/
cp /tmp/imgui-extract/imstb_rectpack.h GsKit/imgui/
cp /tmp/imgui-extract/imstb_textedit.h GsKit/imgui/
cp /tmp/imgui-extract/imstb_truetype.h GsKit/imgui/
cp /tmp/imgui-extract/backends/imgui_impl_sdl2.cpp GsKit/imgui/
cp /tmp/imgui-extract/backends/imgui_impl_sdl2.h GsKit/imgui/
cp /tmp/imgui-extract/backends/imgui_impl_sdlrenderer2.cpp GsKit/imgui/
cp /tmp/imgui-extract/backends/imgui_impl_sdlrenderer2.h GsKit/imgui/
cp /tmp/imgui-extract/LICENSE.txt GsKit/imgui/
rm -rf /tmp/imgui-extract
```

**Step 2: Create CMakeLists.txt for ImGui**

Create `GsKit/imgui/CMakeLists.txt`:
```cmake
project(GsKit_imgui)

set(IMGUI_SOURCES
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_demo.cpp
    imgui_impl_sdl2.cpp
    imgui_impl_sdlrenderer2.cpp
)

add_library(${PROJECT_NAME} STATIC ${IMGUI_SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${SDL2_INCLUDE_DIR}
)

# Suppress warnings in vendored code
target_compile_options(${PROJECT_NAME} PRIVATE -w)
```

**Step 3: Verify it compiles**

Run:
```bash
cd Build && cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && cmake --build . 2>&1 | grep -E 'imgui|error'
```
Expected: `GsKit_imgui` library builds without errors.

**Step 4: Commit**

```bash
git add GsKit/imgui/
git commit -m "vendor: add Dear ImGui v1.91.8 with SDL2 backend"
```

---

### Task 2: Add USE_IMGUI CMake option and link ImGui

**Files:**
- Modify: `GsKit/CMakeLists.txt`
- Modify: `CMakeLists.txt` (root)

**Step 1: Add ImGui subdirectory and option to GsKit/CMakeLists.txt**

After line `add_subdirectory(base)` (line 121), add:
```cmake
option(USE_IMGUI "Enable Dear ImGui UI" ON)
if(USE_IMGUI)
    add_subdirectory(imgui)
    ADD_DEFINITIONS(-DUSE_IMGUI)
    Target_link_libraries(${PROJECT_NAME} GsKit_imgui)
endif()
```

**Step 2: Verify build**

Run:
```bash
cd Build && cmake .. && cmake --build . 2>&1 | tail -10
```
Expected: Build succeeds, `libGsKit_imgui.a` appears in build output.

**Step 3: Verify flag works by disabling it**

Run:
```bash
cmake .. -DUSE_IMGUI=OFF && cmake --build . 2>&1 | tail -5
```
Expected: Build still succeeds without ImGui.

**Step 4: Commit**

```bash
git add GsKit/CMakeLists.txt
git commit -m "build: add USE_IMGUI cmake option, link ImGui to GsKit"
```

---

### Task 3: Initialize ImGui in the SDL video subsystem

**Files:**
- Modify: `GsKit/base/video/CSDLVideo.cpp`
- Modify: `GsKit/base/video/CSDLVideo.h`

**Step 1: Add ImGui init/teardown to CSDLVideo**

In `CSDLVideo.h`, add at top:
```cpp
#ifdef USE_IMGUI
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#endif
```

In `CSDLVideo::init()`, after `SDL_RenderPresent(renderer)` (around line 122), add:
```cpp
#ifdef USE_IMGUI
    // Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    gLogging.textOut(FONTCOLORS::GREEN, "Dear ImGui initialized<br>");
#endif
```

In `CSDLVideo::shutdown()` (or destructor), add cleanup:
```cpp
#ifdef USE_IMGUI
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
#endif
```

**Step 2: Build and verify**

Run:
```bash
cd Build && cmake .. && cmake --build . 2>&1 | tail -10
```
Expected: Build succeeds, "Dear ImGui initialized" appears in log when app starts.

**Step 3: Commit**

```bash
git add GsKit/base/video/CSDLVideo.cpp GsKit/base/video/CSDLVideo.h
git commit -m "feat: initialize Dear ImGui in SDL video subsystem"
```

---

### Task 4: Add ImGui to the render loop (test overlay)

**Files:**
- Modify: `GsKit/base/video/CSDLVideo.cpp` (in `transformScreenToDisplay`)

**Step 1: Add ImGui frame rendering**

In `CSDLVideo::transformScreenToDisplay()`, just before `SDL_RenderPresent(renderer)` (find it near the end of the function), add:

```cpp
#ifdef USE_IMGUI
    // Begin ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Test overlay — remove after Phase A validation
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::Begin("CG Debug", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
                 ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Commander Genius + ImGui");
    ImGui::Text("Frame: %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::End();

    // Render ImGui
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
#endif
```

**Step 2: Add SDL event forwarding to ImGui**

In `GsKit/base/GsApp.cpp` (or wherever `SDL_PollEvent` is called), add inside the event loop:
```cpp
#ifdef USE_IMGUI
    ImGui_ImplSDL2_ProcessEvent(&event);
#endif
```

Find the event polling location:
```bash
grep -rn "SDL_PollEvent" GsKit/base/
```

**Step 3: Build, run, and verify the overlay appears**

Run:
```bash
cd Build && cmake --build . && cd .. && ./Build/src/CGeniusExe
```
Expected: A small "Commander Genius + ImGui" debug window appears in the top-left corner with FPS counter.

**Step 4: Commit**

```bash
git add GsKit/base/video/CSDLVideo.cpp GsKit/base/GsApp.cpp
git commit -m "feat: ImGui test overlay rendering in game loop"
```

---

### Task 5: Set up the polished retro theme

**Files:**
- Create: `GsKit/imgui/CImGuiTheme.h`
- Create: `GsKit/imgui/CImGuiTheme.cpp`

**Step 1: Create the theme configuration**

Create `GsKit/imgui/CImGuiTheme.h`:
```cpp
#pragma once
#ifdef USE_IMGUI

#include "imgui.h"
#include <string>

namespace CImGuiTheme
{
    // Apply the polished retro theme to ImGui
    void apply();

    // Load the pixel font, returns true on success
    bool loadFont(const std::string& fontPath, float sizePixels);

    // Color palette accessors
    ImVec4 background();
    ImVec4 panel();
    ImVec4 accentPrimary();
    ImVec4 accentSecondary();
    ImVec4 text();
    ImVec4 textDimmed();
}

#endif // USE_IMGUI
```

Create `GsKit/imgui/CImGuiTheme.cpp`:
```cpp
#ifdef USE_IMGUI

#include "CImGuiTheme.h"

namespace CImGuiTheme
{

static ImVec4 hexToImVec4(uint32_t hex, float alpha = 1.0f)
{
    return ImVec4(
        ((hex >> 16) & 0xFF) / 255.0f,
        ((hex >> 8) & 0xFF) / 255.0f,
        (hex & 0xFF) / 255.0f,
        alpha
    );
}

ImVec4 background()      { return hexToImVec4(0x1a1a2e); }
ImVec4 panel()           { return hexToImVec4(0x16213e); }
ImVec4 accentPrimary()   { return hexToImVec4(0xe94560); }
ImVec4 accentSecondary() { return hexToImVec4(0x53d769); }
ImVec4 text()            { return hexToImVec4(0xeaeaea); }
ImVec4 textDimmed()      { return hexToImVec4(0x8a8a8a); }

void apply()
{
    ImGuiStyle& style = ImGui::GetStyle();

    // Sharp corners — retro feel
    style.WindowRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.PopupRounding = 0.0f;

    // Thin borders
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;

    // Comfortable spacing
    style.WindowPadding = ImVec2(12, 12);
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(8, 6);
    style.ScrollbarSize = 10.0f;

    // Colors
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_WindowBg]           = panel();
    colors[ImGuiCol_ChildBg]            = hexToImVec4(0x0f1525);
    colors[ImGuiCol_PopupBg]            = panel();
    colors[ImGuiCol_Border]             = hexToImVec4(0x3a4a6e);

    colors[ImGuiCol_Text]               = text();
    colors[ImGuiCol_TextDisabled]       = textDimmed();

    colors[ImGuiCol_FrameBg]            = hexToImVec4(0x0f1525);
    colors[ImGuiCol_FrameBgHovered]     = hexToImVec4(0x1e2d4d);
    colors[ImGuiCol_FrameBgActive]      = hexToImVec4(0x2a3d5e);

    colors[ImGuiCol_TitleBg]            = background();
    colors[ImGuiCol_TitleBgActive]      = hexToImVec4(0x1e1e3a);
    colors[ImGuiCol_TitleBgCollapsed]   = background();

    colors[ImGuiCol_Button]             = hexToImVec4(0x1e2d4d);
    colors[ImGuiCol_ButtonHovered]      = accentPrimary();
    colors[ImGuiCol_ButtonActive]       = hexToImVec4(0xc73a52);

    colors[ImGuiCol_Header]             = hexToImVec4(0x1e2d4d);
    colors[ImGuiCol_HeaderHovered]      = accentPrimary();
    colors[ImGuiCol_HeaderActive]       = hexToImVec4(0xc73a52);

    colors[ImGuiCol_ScrollbarBg]        = hexToImVec4(0x0f1525);
    colors[ImGuiCol_ScrollbarGrab]      = hexToImVec4(0x3a4a6e);
    colors[ImGuiCol_ScrollbarGrabHovered] = accentPrimary();
    colors[ImGuiCol_ScrollbarGrabActive]  = hexToImVec4(0xc73a52);

    colors[ImGuiCol_CheckMark]          = accentSecondary();
    colors[ImGuiCol_SliderGrab]         = accentPrimary();
    colors[ImGuiCol_SliderGrabActive]   = hexToImVec4(0xc73a52);

    colors[ImGuiCol_Separator]          = hexToImVec4(0x3a4a6e);
    colors[ImGuiCol_SeparatorHovered]   = accentPrimary();
    colors[ImGuiCol_SeparatorActive]    = accentPrimary();

    colors[ImGuiCol_Tab]                = hexToImVec4(0x1e2d4d);
    colors[ImGuiCol_TabHovered]         = accentPrimary();
    colors[ImGuiCol_TabSelected]        = hexToImVec4(0x2a3d5e);

    colors[ImGuiCol_NavHighlight]       = accentPrimary();
}

bool loadFont(const std::string& fontPath, float sizePixels)
{
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig config;
    config.PixelSnapH = true;           // Pixel-perfect alignment
    config.OversampleH = 1;             // No anti-aliasing for pixel font
    config.OversampleV = 1;

    ImFont* font = io.Fonts->AddFontFromFileTTF(
        fontPath.c_str(), sizePixels, &config
    );

    if (!font)
    {
        // Fall back to ImGui default font
        io.Fonts->AddFontDefault();
        return false;
    }

    return true;
}

} // namespace CImGuiTheme

#endif // USE_IMGUI
```

**Step 2: Add theme files to CMakeLists.txt**

In `GsKit/imgui/CMakeLists.txt`, add to `IMGUI_SOURCES`:
```cmake
    CImGuiTheme.cpp
```

**Step 3: Apply theme after ImGui init**

In `CSDLVideo::init()`, after `ImGui_ImplSDLRenderer2_Init(renderer)`, add:
```cpp
    #include "CImGuiTheme.h"
    // ... in init():
    CImGuiTheme::apply();
    CImGuiTheme::loadFont("vfsroot/global/fonts/PressStart2P-Regular.ttf", 16.0f);
```

**Step 4: Download the Press Start 2P font**

```bash
mkdir -p vfsroot/global/fonts
curl -L "https://github.com/google/fonts/raw/main/ofl/pressstart2p/PressStart2P-Regular.ttf" \
    -o vfsroot/global/fonts/PressStart2P-Regular.ttf
```

**Step 5: Build and verify the theme**

Run:
```bash
cd Build && cmake --build . && cd .. && ./Build/src/CGeniusExe
```
Expected: The debug overlay now uses the pixel font with dark blue/red color scheme.

**Step 6: Commit**

```bash
git add GsKit/imgui/CImGuiTheme.h GsKit/imgui/CImGuiTheme.cpp GsKit/imgui/CMakeLists.txt
git add vfsroot/global/fonts/PressStart2P-Regular.ttf
git commit -m "feat: polished retro ImGui theme with pixel font"
```

---

## Phase B: Rebuild Launcher in ImGui

### Task 6: Create ImGui launcher skeleton

**Files:**
- Create: `src/engine/CImGuiLauncher.h`
- Create: `src/engine/CImGuiLauncher.cpp`

**Step 1: Create the launcher header**

Create `src/engine/CImGuiLauncher.h`:
```cpp
#pragma once
#ifdef USE_IMGUI

#include "imgui.h"
#include <vector>
#include <string>
#include <memory>

struct GameEntry
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

    // Render the launcher UI — call every frame
    // Returns true if user selected a game to start
    bool render();

    // Get the selected game info after render() returns true
    const GameEntry& getSelectedGame() const { return mGames[mSelectedIndex]; }

private:
    void renderGameList();
    void renderPreviewPanel();
    void renderBottomBar();

    std::vector<GameEntry> mGames;
    int mSelectedIndex = -1;
    int mHoveredIndex = -1;
    bool mStartRequested = false;
};

#endif // USE_IMGUI
```

**Step 2: Create the launcher implementation**

Create `src/engine/CImGuiLauncher.cpp`:
```cpp
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

    GameEntry test;
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
```

**Step 3: Add to CMake**

In `src/engine/CMakeLists.txt`, find the source file list and add:
```cmake
if(USE_IMGUI)
    list(APPEND SRCS CImGuiLauncher.cpp)
endif()
```

You'll need to find where `SRCS` or the engine sources are listed. Search for `CGameLauncher.cpp` in that CMakeLists.txt.

**Step 4: Build and verify**

```bash
cd Build && cmake .. && cmake --build . 2>&1 | tail -10
```
Expected: Compiles without errors.

**Step 5: Commit**

```bash
git add src/engine/CImGuiLauncher.h src/engine/CImGuiLauncher.cpp src/engine/CMakeLists.txt
git commit -m "feat: ImGui launcher skeleton with game list and preview panel"
```

---

### Task 7: Wire ImGui launcher to game scanner

**Files:**
- Modify: `src/engine/CImGuiLauncher.cpp`
- Modify: `src/engine/CImGuiLauncher.h`

**Step 1: Replace test data with real game scanning**

The existing game scanner in `CGameLauncher.cpp` uses `scanExecutables()` and `scanSubDirectories()`. These populate `m_Entries` (vector of `GameEntry` structs). Replicate this logic by:

1. Read `CGameLauncher.h` to understand `GameEntry` struct and `m_Entries`
2. In `CImGuiLauncher::scanForGames()`, use the same VFS search path scanning from `FindFile.h`:
   - Call `GetFirstSearchPath()` to find game directories
   - Use `FindFiles()` to locate `.EXE` files (keen executables)
   - Parse episode numbers from filenames (KEEN1.EXE → episode 1)
3. Load `preview.bmp` from each game directory as an SDL texture for the preview panel

**Step 2: Wire the "Start Game" action**

When `mStartRequested` is true, emit the appropriate engine event:
```cpp
#include <base/interface/event/EventSendStruct.h>

// In the caller that checks render() return:
if (episode >= 1 && episode <= 3)
    gEventManager.add(new StartVorticonEngine(false, episode, directory));
else if (episode >= 4 && episode <= 6)
    gEventManager.add(new StartGalaxyEngine(false, episode, directory));
else if (episode == 7)
    gEventManager.add(new StartDreamsEngine(false, directory));
```

**Step 3: Swap launcher entry point**

In `CGameLauncher.cpp`, wrap the old `init()` method:
```cpp
#ifdef USE_IMGUI
    // ImGui launcher handles everything
    mImGuiLauncher = std::make_unique<CImGuiLauncher>();
    return true;
#else
    // ... existing old launcher code ...
#endif
```

**Step 4: Build, run, and verify games appear**

```bash
cd Build && cmake --build . && cd .. && ./Build/src/CGeniusExe
```
Expected: Real games from `games/` directory appear in the ImGui launcher list. Clicking "Start Game" launches the selected episode.

**Step 5: Commit**

```bash
git add src/engine/CImGuiLauncher.cpp src/engine/CImGuiLauncher.h src/engine/CGameLauncher.cpp
git commit -m "feat: wire ImGui launcher to game scanner and engine events"
```

---

### Task 8: Add preview image loading

**Files:**
- Modify: `src/engine/CImGuiLauncher.cpp`
- Modify: `src/engine/CImGuiLauncher.h`

**Step 1: Load preview.bmp as SDL texture**

Add to `GameEntry`:
```cpp
SDL_Texture* previewTexture = nullptr;
int previewWidth = 0;
int previewHeight = 0;
```

When a game is selected, load its preview:
```cpp
void CImGuiLauncher::loadPreview(int index)
{
    auto& game = mGames[index];
    if (game.previewTexture) return; // already loaded

    std::string path = JoinPaths(game.directory, "preview.bmp");
    SDL_Surface* surface = SDL_LoadBMP(path.c_str());
    if (!surface) return;

    // Get the renderer from the video driver
    SDL_Renderer* renderer = /* get from CVideoDriver singleton */;
    game.previewTexture = SDL_CreateTextureFromSurface(renderer, surface);
    game.previewWidth = surface->w;
    game.previewHeight = surface->h;
    SDL_FreeSurface(surface);
}
```

**Step 2: Render preview texture in ImGui**

Replace the placeholder in `renderPreviewPanel()`:
```cpp
if (game.previewTexture)
{
    float aspect = static_cast<float>(game.previewWidth) / game.previewHeight;
    float dispWidth = ImGui::GetContentRegionAvail().x;
    float dispHeight = dispWidth / aspect;
    ImGui::Image(reinterpret_cast<ImTextureID>(game.previewTexture),
                 ImVec2(dispWidth, dispHeight));
}
```

**Step 3: Clean up textures in destructor**

```cpp
CImGuiLauncher::~CImGuiLauncher()
{
    for (auto& game : mGames)
    {
        if (game.previewTexture)
            SDL_DestroyTexture(game.previewTexture);
    }
}
```

**Step 4: Build, run, verify previews display**

**Step 5: Commit**

```bash
git add src/engine/CImGuiLauncher.cpp src/engine/CImGuiLauncher.h
git commit -m "feat: load and display game preview images in ImGui launcher"
```

---

### Task 9: Add download manager to ImGui launcher

**Files:**
- Modify: `src/engine/CImGuiLauncher.cpp`
- Modify: `src/engine/CImGuiLauncher.h`

**Step 1: Add download button and progress UI**

In the bottom bar, add a "+ Download Games" button that opens a modal dialog. Wire it to the existing `GameDownloader` class (`src/engine/gamedownloader.h`).

The modal shows:
- Available games list (from the game catalog XML)
- Download progress bar per game
- Close button

Use ImGui's `OpenPopup` / `BeginPopupModal` for this.

**Step 2: Wire to existing downloader**

The existing `GameDownloader::downloadGame()` updates a `progress` variable (0-1000 permillage). Poll this in the ImGui render loop to update the progress bar.

**Step 3: Build, test download flow**

**Step 4: Commit**

```bash
git commit -m "feat: download manager modal in ImGui launcher"
```

---

## Phase C: Rebuild In-Game Menus

### Task 10: Create ImGui in-game menu overlay

**Files:**
- Create: `src/engine/core/menu/CImGuiGameMenu.h`
- Create: `src/engine/core/menu/CImGuiGameMenu.cpp`

**Step 1: Create the menu overlay**

The in-game menu renders as a semi-transparent overlay on the paused game frame. Create `CImGuiGameMenu` with:

```cpp
class CImGuiGameMenu
{
public:
    enum class Screen { MAIN, VIDEO, AUDIO, CONTROLS, GAME_OPTIONS, SAVE, LOAD };

    void open();
    void close();
    bool isOpen() const { return mIsOpen; }

    // Call every frame — renders nothing if closed
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
```

**Step 2: Implement main menu screen**

```cpp
void CImGuiGameMenu::renderMainMenu()
{
    // Centered panel, 300px wide
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(300, 0));

    ImGui::Begin("##GameMenu", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushStyleColor(ImGuiCol_Text, CImGuiTheme::accentPrimary());
    float textWidth = ImGui::CalcTextSize("PAUSED").x;
    ImGui::SetCursorPosX((300 - textWidth) / 2);
    ImGui::Text("PAUSED");
    ImGui::PopStyleColor();

    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Continue", ImVec2(-1, 0)))   close();
    if (ImGui::Button("Save Game", ImVec2(-1, 0)))   mCurrentScreen = Screen::SAVE;
    if (ImGui::Button("Load Game", ImVec2(-1, 0)))   mCurrentScreen = Screen::LOAD;
    if (ImGui::Button("Settings", ImVec2(-1, 0)))    mCurrentScreen = Screen::VIDEO;
    if (ImGui::Button("Quit to Menu", ImVec2(-1, 0)))
    {
        gEventManager.add(new EventEndGamePlay());
        close();
    }

    ImGui::End();
}
```

**Step 3: Hook into game loop**

In the main `ponder()`/`render()` cycle, when ESC is pressed:
```cpp
#ifdef USE_IMGUI
    if (mImGuiMenu.isOpen())
    {
        mImGuiMenu.render();
        return; // don't update game logic while paused
    }
#endif
```

**Step 4: Build and verify the overlay**

**Step 5: Commit**

```bash
git add src/engine/core/menu/CImGuiGameMenu.h src/engine/core/menu/CImGuiGameMenu.cpp
git commit -m "feat: ImGui in-game menu overlay with main menu screen"
```

---

### Task 11: Implement video settings screen

**Files:**
- Modify: `src/engine/core/menu/CImGuiGameMenu.cpp`

**Step 1: Read current video settings from CVidConfig**

The existing `DisplaySettings.cpp` reads from `gVideoDriver.getVidConfig()`. Use the same accessors.

**Step 2: Build the video settings UI**

```cpp
void CImGuiGameMenu::renderVideoSettings()
{
    // ... window setup ...

    ImGui::Text("VIDEO SETTINGS");
    ImGui::Separator();

    // Fullscreen toggle
    bool fullscreen = gVideoDriver.getVidConfig().mFullscreen;
    if (ImGui::Checkbox("Fullscreen", &fullscreen))
    {
        // Apply change via existing event system
    }

    // VSync
    bool vsync = gVideoDriver.getVidConfig().mVSync;
    if (ImGui::Checkbox("VSync", &vsync)) { /* apply */ }

    // Resolution combo
    // Filter combo
    // Integer scaling checkbox

    if (ImGui::Button("Back", ImVec2(-1, 0)))
        mCurrentScreen = Screen::MAIN;
}
```

**Step 3: Wire setting changes to existing event system**

Use the same `ChangeVideoSettingsEvent` pattern from `DisplaySettings.cpp`.

**Step 4: Build, test changing settings**

**Step 5: Commit**

```bash
git commit -m "feat: ImGui video settings screen"
```

---

### Task 12: Implement audio settings screen

Same pattern as Task 11 but for audio: sample rate, music volume, sound volume, stereo toggle. Wire to existing `CSettings` and `CSound` APIs.

```bash
git commit -m "feat: ImGui audio settings screen"
```

---

### Task 13: Implement controls/input settings screen

Display current key bindings in a table. Allow rebinding by clicking a key slot and pressing a new key. Wire to existing input mapping system in `GsKit/base/CInput.h`.

```bash
git commit -m "feat: ImGui controls settings screen"
```

---

### Task 14: Implement save/load game screens

**Save screen:** List of save slots with timestamps. Text input for slot name. Save button emits existing save event.

**Load screen:** List of saved games with names and timestamps. Load button emits existing load event.

Wire to `CSaveGameController` API.

```bash
git commit -m "feat: ImGui save and load game screens"
```

---

## Phase D: Remove Old Widget Code

### Task 15: Remove old launcher behind USE_IMGUI

**Files:**
- Modify: `src/engine/CGameLauncher.cpp`
- Modify: `src/engine/CGameLauncher.h`

**Step 1: Wrap all old launcher code in `#ifndef USE_IMGUI`**

Keep the game scanning logic (it's reused), but wrap the dialog/widget creation code.

**Step 2: Wrap old menu classes**

In each file in `src/engine/core/menu/`:
```cpp
#ifndef USE_IMGUI
// ... entire old menu class ...
#endif
```

**Step 3: Build with USE_IMGUI=ON (should work)**
**Step 4: Build with USE_IMGUI=OFF (should still work with old UI)**
**Step 5: Commit**

```bash
git commit -m "refactor: gate old widget UI behind #ifndef USE_IMGUI"
```

---

### Task 16: Remove USE_IMGUI flag — ImGui is the default

**Files:**
- Modify: `GsKit/CMakeLists.txt`
- Modify: All files with `#ifdef USE_IMGUI`

**Step 1: Remove the option — ImGui is always on**

Delete the `option(USE_IMGUI ...)` line. Always include ImGui. Remove all `#ifdef USE_IMGUI` / `#ifndef USE_IMGUI` guards.

**Step 2: Delete old widget classes that are fully replaced**

Remove files that are no longer compiled:
- `GsKit/widgets/GsButton.*` (if fully replaced)
- `GsKit/widgets/GsDialog.*` (if fully replaced)
- Old menu files in `src/engine/core/menu/` that are superseded

Keep `GsKit/widgets/` classes that are used by game rendering (not UI menus).

**Step 3: Full build and test**

**Step 4: Commit**

```bash
git commit -m "refactor: remove USE_IMGUI flag, ImGui is the standard UI"
```

---

## Testing Checklist

After each phase, verify:

### Phase A
- [ ] ImGui debug overlay renders on top of the existing launcher
- [ ] FPS counter visible and accurate
- [ ] Retro theme colors visible (dark blue background, pixel font)
- [ ] Build passes with `USE_IMGUI=ON` and `USE_IMGUI=OFF`

### Phase B
- [ ] All installed games appear in the ImGui launcher list
- [ ] Selecting a game shows its preview image and info
- [ ] Double-clicking or pressing "Start Game" launches the correct episode
- [ ] Keen 1-3 launch Vorticon engine, Keen 4-6 launch Galaxy engine, Dreams launches Dreams engine
- [ ] Settings button opens settings (even if placeholder)
- [ ] Download games modal works (if DOWNLOADER is enabled)
- [ ] Keyboard navigation works (arrow keys + enter)
- [ ] Quit button exits the application

### Phase C
- [ ] ESC pauses game and shows the ImGui overlay
- [ ] Continue resumes gameplay
- [ ] Save/Load screens show correct slot info
- [ ] Video settings changes apply in real time
- [ ] Audio settings changes apply in real time
- [ ] Key rebinding works
- [ ] Quit to Menu returns to launcher

### Phase D
- [ ] Build succeeds with only ImGui code
- [ ] No references to old widget classes in compiled code
- [ ] Binary size is similar or smaller than before
- [ ] All functionality from old UI is preserved
