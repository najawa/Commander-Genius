# Dear ImGui UI Overhaul — Design Document

**Date:** 2026-03-08
**Status:** Approved
**Designer:** najawa

---

## Goal

Replace the existing GsKit widget/menu system with Dear ImGui to achieve a polished retro aesthetic across the launcher and all in-game menus.

## Architecture

Dear ImGui integrated as an overlay renderer on top of the existing SDL2/OpenGL pipeline. The game rendering pipeline stays untouched — ImGui draws the UI layer on top. When in the launcher (no game running), ImGui renders the full screen.

### Integration Points

- **Vendored in:** `GsKit/imgui/` (MIT license, ~35 files)
- **Backends:** `imgui_impl_sdl2` + `imgui_impl_opengl3` (with `imgui_impl_sdlrenderer2` as software fallback)
- **Init/teardown:** `GsKit/base/video/CSDLVideo.cpp`
- **CMake flag:** `USE_IMGUI` (default ON), with `#ifdef`/`#ifndef` guards for coexistence during migration

### Event Bridge

ImGui menus emit the same C++ events the old system uses (`StartGameEvent`, `ChangeVideoSettingsEvent`, etc.). No changes to engine internals — only the presentation layer swaps.

---

## Visual Design — Polished Retro

### Color Palette

| Role | Hex | Description |
|------|-----|-------------|
| Background | `#1a1a2e` | Deep navy |
| Panel | `#16213e` | Dark blue-gray with 1px bright border |
| Accent (primary) | `#e94560` | Warm red — selection, active buttons |
| Accent (secondary) | `#53d769` | Pixel green — confirmations |
| Text | `#eaeaea` | Soft white |
| Text (dimmed) | `#8a8a8a` | Gray for secondary info |

### Typography

- **Primary:** "Press Start 2P" (OFL licensed) at native pixel sizes, no anti-aliasing
- **Secondary:** Same family at smaller size for labels/info
- Loaded via ImGui's `AddFontFromFileTTF()` at exact pixel sizes

### Widget Style

- **Buttons:** Rectangular, 1-2px bright border, color shift on hover (no rounded corners)
- **Lists:** Accent-highlighted rows, scanline-style alternating tints
- **Panels:** Sharp corners, thin bright borders
- **Scrollbars:** Thin, palette-matched
- **Transitions:** Snappy 2-3 frame color transitions

---

## Menu Structure

### Launcher

- Full-screen ImGui window
- Left panel: Game list with episode thumbnails
- Right panel: Preview image, episode info, "Start" button
- Bottom toolbar: Settings, Downloads, About

### In-Game Menu

- Semi-transparent overlay on paused game frame
- Unified style regardless of engine (Galaxy/Vorticon/Dreams)
- Menu tree: New Game, Load, Save, Configure, Quit
- Configure: Video, Audio, Controls, Game Options

### Input

- Keyboard, gamepad, mouse, and touch all supported via ImGui's SDL2 backend
- ImGui's built-in keyboard/gamepad navigation

---

## Migration Strategy

Phased approach — old and new UI coexist:

| Phase | Scope | Outcome |
|-------|-------|---------|
| A | Integrate ImGui, test overlay | ImGui renders on existing launcher |
| B | Rebuild launcher in ImGui | Full game picker, preview, settings access |
| C | Rebuild in-game menus | Settings, save/load, controls in ImGui |
| D | Remove old widget code | Clean codebase, `USE_IMGUI` flag removed |

Each phase produces a working build.

---

## Dependencies

| Dependency | License | Size | Notes |
|-----------|---------|------|-------|
| Dear ImGui | MIT | ~60KB source | Vendored in GsKit/imgui/ |
| Press Start 2P font | OFL | ~20KB | Bundled in vfsroot/global/fonts/ |

No new system-level dependencies. MIT is GPL v2 compatible.
