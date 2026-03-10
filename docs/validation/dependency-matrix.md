# Commander Genius — Dependency Matrix

**Phase:** 0 (Inventory & Triage)
**Date:** 2026-03-07

---

## Build Requirements

| Requirement | Version | Enforcement |
|-------------|---------|-------------|
| CMake | >= 3.12 | Hard (CMakeLists.txt line 3) |
| C++ Standard | C++17 | Hard (CMAKE_CXX_STANDARD 17) |
| C Standard | C99 | Hard (Lua and RefKeen code) |

---

## External Dependencies

| Dependency | Min Version | Declared In | Required? | Platforms | Notes |
|------------|-------------|-------------|-----------|-----------|-------|
| **SDL2** | 2.0+ (soft) | `src/CMakeLists.txt:73` | Yes | All | Core graphics/input/window |
| **SDL2_image** | 2.0+ (soft) | `src/CMakeLists.txt:74` | Yes | All | Image format loading |
| **SDL2_mixer** | 2.0+ (soft) | `src/CMakeLists.txt:75` | Yes | All | Audio playback |
| **SDL2_ttf** | 2.0+ (soft) | `src/CMakeLists.txt:75` | Yes | All | TrueType font rendering |
| **curl** | >= 7.32 | `src/engine/CMakeLists.txt:49-50` | Conditional | All (if DOWNLOADER=ON) | Game catalogue downloads |
| **zlib** | Any | `src/engine/unzip/CMakeLists.txt:25` | Yes | All | Compression |
| **OpenGL** | Any | `GsKit/CMakeLists.txt:111` | Optional | Linux, macOS, Windows | Hardware-accelerated rendering |
| **Boost** | Any | `GsKit/CMakeLists.txt:28` | Optional | All (default OFF) | XML parsing; falls back to tinyxml2 |

---

## iOS Dependencies (via FetchContent)

All dependencies are statically compiled for iOS. No system packages required.

| Dependency | Pinned Version | Source |
|------------|---------------|--------|
| SDL2 | 2.30.10 | GitHub release tag |
| SDL2_image | 2.8.2 | GitHub release tag |
| SDL2_mixer | 2.8.1 | GitHub release tag |
| SDL2_ttf | 2.24.0 | GitHub release tag |
| curl | 8.11.1 | GitHub release tag |

### iOS Build Notes

- All libs built as static (no dynamic linking)
- SDL2_DISABLE_SDL2MAIN=ON
- SDL2_mixer: OPUS, MOD_XMP, MOD, FLUIDSYNTH, WAVPACK disabled
- SDL2_ttf: vendored FreeType enabled
- curl: uses SecureTransport (not OpenSSL), LDAP/LDAPS disabled

---

## Vendored / Bundled Code

| Component | Location | Purpose | Upstream | CVE Status |
|-----------|----------|---------|----------|------------|
| **Lua** | `GsKit/base/lua/` | Scripting engine | Lua 5.3+ (exact version TBD) | NEEDS AUDIT |
| **RefKeen** | `src/engine/refkeen/` | Keen Dreams engine | NY00123/refkeen | NEEDS AUDIT |
| **minizip/unzip** | `src/engine/unzip/` | ZIP extraction | minizip (zlib contrib) | NEEDS AUDIT |
| **depklite** | `src/engine/depklite/` | EXE decompression | Custom/original | NEEDS AUDIT |
| **tinyxml2** | `GsKit/base/interface/property_tree/` | XML parsing (Boost fallback) | tinyxml2 | NEEDS AUDIT |
| **dbopl** | `GsKit/base/audio/base/` | OPL synth emulation | DOSBox OPL | NEEDS AUDIT |

---

## Platform Build Toolchains

| Platform | Compiler | Toolchain File | Docker Image (CI) |
|----------|----------|---------------|-------------------|
| Linux | GCC 14+ / Clang | — | `cgenius/crossbuild:linux` |
| Windows | MinGW-w64 (MSYS2) | `toolchain-mingw.cmake` | `cgenius/crossbuild:winsdl` / `winnsis` |
| macOS | Apple Clang / osxcross | osxcross toolchain | `cgenius/crossbuild:mac` |
| iOS | Xcode (Apple Clang) | iOS toolchain | — (local build) |
| Android | NDK | Android toolchain | — |
| Nintendo Switch | devkitPro | Switch toolchain | — |
| GP2X/Wiz/Caanoo | ARM cross-compiler | `toolchains/` | — (DEPRECATED) |

---

## Package Dependencies (Distribution)

### Debian/Ubuntu (.deb)

```
libsdl2-2.0-0 (>= 2.0.0)
libsdl2-image-2.0-0 (>= 2.0)
libsdl2-mixer-2.0-0 (>= 2.0)
libsdl2-ttf-2.0-0 (>= 2.0)
```

### Fedora/RHEL (.rpm)

```
SDL >= 2.0
SDL_image >= 2.0
SDL_mixer >= 2.0
mesa-libGL >= 7.7
```

---

## CMake Feature Flags

| Flag | Default | Effect | Dependencies Affected |
|------|---------|--------|----------------------|
| `BUILD_KEEN` | ON | Build Keen engine (1-6) | — |
| `BUILD_REFKEEN` | ON | Build RefKeen for Keen Dreams | — |
| `BUILD_COSMOS` | OFF | Build Cosmos engine (INCOMPLETE) | cosmos submodule |
| `DOWNLOADER` | ON | Enable game catalogue downloader | curl >= 7.32 |
| `USE_VIRTUALPAD` | ON | On-screen gamepad for touch | — |
| `USE_OPENGL` | varies | OpenGL rendering path | OpenGL |
| `USE_BOOST` | OFF | Boost XML support | Boost |
| `DISABLE_HOVER` | OFF | Disable hover effects (touch) | — |
| `EMBEDDED` | OFF | Embedded system build | — |
| `NINTENDO_SWITCH` | OFF | Switch platform build | devkitPro |
| `APPEND_SHA` | OFF | Append git SHA to version | git |
| `CREATE_DEBS` | OFF | Build .deb packages | dpkg-deb |
| `CREATE_RPMS` | OFF | Build .rpm packages | rpmbuild |

---

## Dependency Graph

```
Commander Genius (src/)
  |
  +-- GsKit/ (framework)
  |     +-- SDL2 (required)
  |     +-- SDL2_image (required)
  |     +-- SDL2_mixer (required)
  |     +-- SDL2_ttf (required)
  |     +-- OpenGL (optional)
  |     +-- Boost (optional, falls back to vendored tinyxml2)
  |     +-- Lua (vendored)
  |     +-- dbopl (vendored, OPL emulation)
  |
  +-- curl (conditional: DOWNLOADER=ON)
  +-- zlib (required)
  +-- minizip (vendored, uses zlib)
  +-- depklite (vendored)
  +-- RefKeen (vendored)
  +-- Cosmos (submodule, incomplete)
```

---

## Action Items

- [ ] Identify exact vendored Lua version and audit against CVEs
- [ ] Identify vendored minizip version and audit against CVEs
- [ ] Identify vendored tinyxml2 version and audit against CVEs
- [ ] Audit iOS FetchContent pinned versions (SDL2 2.30.10, curl 8.11.1) for known CVEs
- [ ] Enforce minimum SDL2 version in CMake (currently soft "2.0+")
- [ ] Consider updating vendored components to latest stable releases
