# Commander Genius — Build Matrix

**Phase:** 1 (Build System Validation)
**Date:** 2026-03-07
**Last Updated:** 2026-03-07

---

## Platform Build Results

| Platform | Compiler | Status | Warnings | Binary Size | Notes |
|----------|----------|--------|----------|-------------|-------|
| macOS (ARM64) | Apple Clang 17.0.0 | PASS | 3 (non-source) | 4.4MB | Primary validation platform |
| Linux (x86_64) | GCC 14 | Deferred | — | — | Needs CI setup |
| Linux (x86_64) | Clang (latest) | Deferred | — | — | Needs CI setup |
| Windows (x86_64) | MinGW-w64 (MSYS2) | Deferred | — | — | Needs CI setup |
| Android | NDK | Deferred | — | — | Needs CI setup |
| iOS | Xcode (Apple Clang) | Deferred | — | — | Needs Xcode project validation |
| Nintendo Switch | devkitPro | Deferred | — | — | Needs devkit access |

---

## CMake Option Combination Results (macOS)

| Configuration | CMake Status | Build Status | Notes |
|---------------|-------------|-------------|-------|
| Default (KEEN=ON, REFKEEN=ON, COSMOS=OFF) | PASS | PASS | Primary configuration |
| BUILD_REFKEEN=OFF | PASS | PASS | Fixed: guarded Dreams code behind `#ifdef BUILD_REFKEEN` |
| DOWNLOADER=OFF | PASS | PASS | Disables curl dependency |
| USE_VIRTUALPAD=OFF | PASS | PASS | Disables touch gamepad |
| USE_OPENGL=OFF | PASS | PASS | Software rendering only |
| BUILD_COSMOS=ON | PASS | Not tested | Cosmos engine is incomplete stub |

---

## Build Fixes Applied

### 1. BUILD_REFKEEN=OFF Linker Failure (Fixed)

**Problem:** `CGameLauncher.cpp` referenced `DreamsEngine` without `#ifdef BUILD_REFKEEN` guards, causing undefined symbol errors when RefKeen was disabled.

**Fix:** Added `#ifdef BUILD_REFKEEN` guards around:
- `#include "keen/dreams/dreamsengine.h"` (line 46)
- `kdreams` detection code (line 447)
- `StartDreamsEngine` event dispatch (line 970)

**File:** `src/engine/CGameLauncher.cpp`

### 2. xxd.c Deprecated Prototypes (Fixed)

**Problem:** `strtol()` and `ftell()` declared without prototypes (K&R style).

**Fix:** Added proper ANSI C prototypes:
```c
extern long int strtol(const char *, char **, int);
extern long int ftell(FILE *);
```

**File:** `src/engine/xxd.c`

### 3. GsLogging vsprintf Deprecation (Fixed)

**Problem:** `vsprintf()` deprecated on macOS, recommends `vsnprintf()`.

**Fix:** Replaced all 3 calls with `vsnprintf(buffer.data(), buffer.size(), Text, pArgList)`.

**File:** `GsKit/base/GsLogging.cpp`

---

## Warning Suppression for Vendored Code

Vendored third-party code uses legacy C patterns intentionally. Warnings are suppressed at the CMake level rather than modifying vendored source.

| Vendored Module | Warnings Suppressed | CMake File |
|----------------|--------------------|----|
| RefKeen (be_cross.h, kdreams/) | `-Wno-invalid-source-encoding`, `-Wno-deprecated-declarations` | `src/engine/refkeen/CMakeLists.txt` |
| RefKeen/kdreams | `-Wno-invalid-source-encoding`, `-Wno-string-plus-int`, `-Wno-deprecated-declarations` | `src/engine/refkeen/kdreams/CMakeLists.txt` |
| keen/dreams (includes refkeen headers) | `-Wno-deprecated-declarations` | `src/engine/keen/dreams/CMakeLists.txt` |
| minizip/unzip | `-Wno-deprecated-non-prototype` | `src/engine/unzip/CMakeLists.txt` |

---

## Warning Reduction Progress

| Stage | Total Warnings | Delta |
|-------|---------------|-------|
| Initial build | 422 | — |
| After vendored code suppression | 32 | -390 |
| After override + std::move fixes | 3 | -29 |
| **Final** | **3** (non-source) | **-419** |

### Remaining Warnings (3 — all non-source-code)

| Warning Type | Count | Owner | Status |
|-------------|-------|-------|--------|
| Empty archive (libGsKit.a) | 1 | CMake config | Accepted — meta-target by design |
| Duplicate libraries (linker) | 1 | CMake config | Accepted — circular deps, harmless |
| `-W#warnings` (CrashHandler.cpp) | 1 | Intentional | Accepted — ARM64 backtrace info |

---

## Minimum Version Requirements

| Component | Minimum Version | Validated Version | Notes |
|-----------|----------------|-------------------|-------|
| CMake | 3.12 | 4.2.3 | Hard requirement in CMakeLists.txt |
| C++ Standard | C++17 | C++17 | Required for `std::optional`, `std::string_view`, etc. |
| C Standard | C99 | C99 | Required for Lua and RefKeen code |
| GCC | 4.8 (per README) | Not tested | Likely needs >=8 for full C++17 |
| Clang | (not specified) | 17.0.0 | Apple Clang 17 validated |
| SDL2 | 2.0+ | 2.32.10 | No enforced minimum in CMake |
| SDL2_image | 2.0+ | 2.8.8 | No enforced minimum in CMake |
| SDL2_mixer | 2.0+ | 2.8.1 | No enforced minimum in CMake |
| SDL2_ttf | 2.0+ | 2.24.0 | No enforced minimum in CMake |
| curl | 7.32 | 8.7.1 | Hard check in CMake |
| zlib | (any) | 1.2.12 | No version check |
| macOS SDK | (not specified) | 15.4 | Deployment target in CI |

---

## Linker Issues (Pending)

### 1. Duplicate Library Warnings

The linker reports duplicate libraries being passed. Root cause investigation in progress — likely due to transitive `target_link_libraries` chains in CMake creating redundant entries.

### 2. Empty libGsKit.a Archive

`libGsKit.a` has no object files. The GsKit target appears to be a meta-target that links its sub-libraries (GsKit_base, GsKit_graphics, etc.) but has no source files of its own.
