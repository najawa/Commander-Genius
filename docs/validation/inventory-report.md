# Commander Genius — Inventory Report

**Phase:** 0 (Inventory & Triage)
**Date:** 2026-03-07

---

## 1. Source File Catalog

### Code Metrics

| Component | Files | Lines of Code | Language |
|-----------|-------|---------------|----------|
| `src/` (game engine) | 586 | ~87,500 | C++17, C99 |
| `GsKit/` (framework) | 240 | ~55,700 | C++17, C99 |
| **Total** | **826** | **~143,200** | |

### Source Directory Map

```
src/
  engine/
    core/             Core engine: game loop, camera, collision, HUD, settings
      menu/           40+ menu UI classes (audio, display, game settings, controls)
      mode/           Game modes/scenes (play, cutscene, demo, etc.)
      VGamepads/      On-screen virtual gamepad support for touch devices
    keen/
      vorticon/       Keen 1-3 engine (Vorticon series, CGA/EGA graphics)
      galaxy/         Keen 4-6 engine (Galaxy series, EGA advanced)
        ep4/          Episode 4 specific enemies/logic
        ep5/          Episode 5 specific enemies/logic
        ep6/          Episode 6 specific enemies/logic
        common/       Shared Galaxy engine code
        ai/           Enemy AI implementations
      dreams/         Keen Dreams (CG wrapper around RefKeen)
    refkeen/          RefKeen engine port for Keen Dreams (vendored)
      kdreams/        Keen Dreams game logic
    cosmos/           Cosmos Cosmic Adventure (INCOMPLETE — stub code only)
    unzip/            ZIP file extraction (vendored minizip)
    depklite/         EXE decompression (PKWARE depacker)
    CGameLauncher.*   Game launcher/selection UI
    gamedownloader.*  Online game catalogue & download
    downloadgui.*     Download progress UI
  fileio/
    CExeFile.*        DOS EXE file parsing (Keen game data)
    CTileLoader.*     Tile and sprite data loading
    CPatcher.*        Game patching system (hex patch support)
    CSaveGameController.*  Save/load game state serialization
    compression/      LZ compression algorithms
    crc.cpp           CRC checksum
  dialog/
    CMessageBox.*     In-game message boxes
    CTextViewer.*     Text display dialogs
    CDlgFrame.*       Dialog frame rendering
  sys/
    wizgp2x.*         DEPRECATED: GP2X/Wiz embedded platform support
  CGenius.cpp         Main entry point
  CGeniusEntry.cpp    Application initialization, platform detection

GsKit/
  base/
    interface/        Low-level abstractions (FindFile, StringUtils, Unicode, Debug)
    video/            Video system (CVideoDriver, CVideoEngine, scaling)
    audio/            Audio system (mixer, OPL emulation via dbopl)
    lua/              Lua 5.3+ scripting (VENDORED — full source)
    CInput.*          Input system (keyboard, joystick, touch)
    GsApp.*           Application lifecycle
    GsEngine.*        Engine base class
    GsEvent.*         Event system
    GsLogging.*       Logging framework
    GsTimer.*         Frame timing
  graphics/
    GsFont.*          Font rendering (TrueType + legacy bitmap)
    GsSurface.*       SDL2 surface abstraction
    GsTexture.*       SDL2 texture abstraction
    GsSprite.*        Sprite management
    GsTilemap.*       Tilemap rendering
    GsGraphics.*      Graphics manager
    Palette, Cursor, Scaling, Effects
  fileio/
    CConfiguration.*  INI-style configuration management
    ResourceMgmt.*    Resource loading
  widgets/
    GsDialog, GsButton, GsCheckbox, GsInputText, GsList,
    GsScrollbar, GsProgressbar, GsFrame, GsText, etc.
  CMake/
    FindSDL2*.cmake   Custom CMake find modules for SDL2 stack
```

---

## 2. Build Status

### Local Build Results (macOS — Darwin 25.3.0)

| Platform | Compiler | Status | Binary | Warnings |
|----------|----------|--------|--------|----------|
| macOS | Apple Clang 17.0.0 | **SUCCESS** | 4.4MB arm64 Mach-O | 422 |
| Linux (GCC 14) | — | Deferred | — | — |
| Linux (Clang) | — | Deferred | — | — |
| Windows (MSYS2) | — | Deferred | — | — |

**Build command:**
```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_BOOST=0 -DBUILD_COSMOS=0
make -j$(sysctl -n hw.ncpu)
```

**Dependencies installed (Homebrew):**
- SDL2 2.32.10, SDL2_image 2.8.8, SDL2_mixer 2.8.1, SDL2_ttf 2.24.0
- curl 8.7.1 (system), zlib 1.2.12 (system)

**Output binary:** `Build/src/CGeniusExe`

### Compiler Warning Breakdown (422 total)

| Warning Type | Count | Phase to Address |
|-------------|-------|-----------------|
| `-Winvalid-source-encoding` | 244 | Phase 3 (encoding cleanup) |
| `-Wdeprecated-declarations` | 96 | Phase 3 (modernization) |
| `-Wunqualified-std-cast-call` | 32 | Phase 3 (`std::move` qualification) |
| `-Winconsistent-missing-override` | 31 | Phase 3 (add `override` keyword) |
| `-Wstring-plus-int` | 12 | Phase 3 (string arithmetic) |
| `-Wdeprecated-non-prototype` | 3 | Phase 3 (C function prototypes) |
| Linker: duplicate libraries | 1 | Phase 1 (CMake cleanup) |
| Linker: empty archive (libGsKit.a) | 1 | Phase 1 (CMake cleanup) |
| Other | 2 | Phase 3 |

### CI/CD Status

**Repo is being moved to new hosting.** Existing GitLab CI and GitHub Actions pipelines will not carry over. CI/CD will be built from scratch in Phase 6.

---

## 3. GsKit Framework Boundary

### Ownership

GsKit is **directly integrated** as a subdirectory — it is NOT a git submodule. It shares the same repo and commit history.

### What GsKit Provides

| Module | Purpose |
|--------|---------|
| `base/CInput` | Keyboard, joystick, touch input handling |
| `base/video/` | SDL2 video rendering, mode management, scaling |
| `base/audio/` | Music and SFX via SDL2_mixer + OPL emulation |
| `base/lua/` | Lua 5.3+ scripting engine (vendored source) |
| `base/GsEvent` | Event-driven architecture |
| `base/GsLogging` | Logging framework |
| `base/GsTimer` | Frame timing and tick management |
| `graphics/` | Fonts, surfaces, textures, sprites, tilemaps, effects |
| `fileio/` | Configuration files, resource loading |
| `widgets/` | Complete UI toolkit (dialogs, buttons, text input, lists) |

### Build Structure

GsKit builds as multiple static libraries:
- `GsKit_base`, `GsKit_base_interface`, `GsKit_base_video`
- `GsKit_base_audio`, `GsKit_base_lua`
- `GsKit_fileio`, `GsKit_graphics`, `GsKit_widgets`

### Only Git Submodule

- **`src/engine/cosmos`** — points to `https://gitlab.com/Dringgstein/cosmos.git` (incomplete)

---

## 4. Legacy Platform Status

### Platforms Marked for Deprecation

| Platform | Files Affected | Separation Quality |
|----------|---------------|-------------------|
| GP2X | `src/sys/wizgp2x.*`, 8 files with `#ifdef` blocks | Clean — removable |
| WIZ | Same as GP2X (shared code) | Clean — removable |
| CAANOO | 5 files with `#ifdef` blocks | Clean — removable |
| DINGOO | 3 files with `#ifdef` blocks | Clean — removable |
| NANONOTE | 2 files with `#ifdef` blocks | Clean — removable |
| PANDORA | 1 file with `#ifdef` block | Clean — removable |

**Assessment:** Legacy platform code is well-contained behind `#ifdef` guards. No interleaving with active platform code. Safe for deprecation or removal.

### Files to Delete if Removing Legacy Support

- `src/sys/wizgp2x.h`
- `src/sys/wizgp2x.cpp`
- Conditional blocks in 8 framework files

---

## 5. Experimental/Incomplete Features

### BUILD_COSMOS (Default: OFF)

**Status:** Infrastructure stub only — no actual engine implementation exists.

- Game detection code exists in `CGameLauncher.cpp` (detects cosmo1/2/3.exe)
- Event handling and launcher integration scaffolded
- `src/engine/cosmos/` directory is a **git submodule pointing to an external repo**
- No CosmoEngine class implementation found locally

**Recommendation:** Keep disabled. Do not invest validation effort until feature is implemented.

---

## 6. Known Technical Debt (TODO/FIXME/HACK)

**Total markers found:** 150+

### Critical Items

| File | Issue |
|------|-------|
| `src/engine/refkeen/be_st_sdl_audio_timer.cpp:1` | "Big TODO: Rework this routine so it better fits to CG" |
| `src/engine/refkeen/be_st_sdl_audio_timer.cpp:405` | "BIG BIG FIXME: OPL emulator called from wrong place" |
| `src/fileio/CSaveGameController.cpp:422` | "Decompression has still to be done!" |
| `src/fileio/CSaveGameController.cpp:494` | "Compression has still to be done!" |
| `src/engine/keen/vorticon/finale/finale.cpp:98` | "Here is big bad bug which makes the game crash..." |
| `src/engine/refkeen/be_st_sdl_graphics.cpp:1533` | "Text mode Broken for some reason, check!" |

### RefKeen Integration

The RefKeen (Keen Dreams) integration has **20+ FIXME comments** in `be_st_sdl.cpp` alone, primarily around:
- Control scheme mapping
- Configuration file handling
- Trigger/axis input mapping

### Incomplete Internationalization

- `GsKit/base/interface/Unicode.cpp:370` — "finish cyrilic"
- `GsKit/base/interface/Unicode.cpp:387` — "hebrew"
