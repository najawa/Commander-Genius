# Commander Genius — Risk Register

**Phase:** 2 (Security Audit & Remediation)
**Date:** 2026-03-07
**Last Updated:** 2026-03-08

---

## Risk Severity Definitions

| Severity | Definition |
|----------|------------|
| **CRITICAL** | Exploitable vulnerability in network-facing or user-input code |
| **HIGH** | Vulnerability in file parsing / deserialization of untrusted data |
| **MEDIUM** | Unsafe pattern in internal game logic or configuration parsing |
| **LOW** | Unsafe pattern in debug/logging code or vendored libraries |

---

## 1. Network Security — Game Downloader

| ID | Risk | Severity | File | Status |
|----|------|----------|------|--------|
| NET-001 | SSL certificate verification disabled (`CURLOPT_SSL_VERIFYPEER, 0L`) — MITM attack vector | **CRITICAL** | `src/engine/gamedownloader.cpp:143` | **FIXED** |
| NET-002 | TLS transport disabled (`CURLOPT_USE_SSL, CURLUSESSL_NONE`) — plaintext downloads | **CRITICAL** | `src/engine/gamedownloader.cpp:145` | **FIXED** |
| NET-003 | Default download URL uses HTTP unless `FORCE_HTTPS` defined | **CRITICAL** | `src/engine/gamedownloader.cpp:107` | **FIXED** |
| NET-004 | No redirect limit — infinite redirect loop / redirect to malicious host | **CRITICAL** | `src/engine/gamedownloader.cpp:141` | **FIXED** |
| NET-005 | No connection/transfer timeouts — hung connections freeze application | **CRITICAL** | `src/engine/gamedownloader.cpp` | **FIXED** |
| NET-006 | No download integrity verification (no checksum validation) | HIGH | `src/engine/gamedownloader.cpp` | Open |
| NET-007 | No download size limit — disk exhaustion possible | MEDIUM | `src/engine/gamedownloader.cpp` | Open |
| NET-008 | Temporary file left on download failure | MEDIUM | `src/engine/gamedownloader.cpp:122` | Open |
| NET-009 | No User-Agent header set | LOW | `src/engine/gamedownloader.cpp` | Open |

---

## 2. Scripting Security — Lua Engine

| ID | Risk | Severity | File | Status |
|----|------|----------|------|--------|
| LUA-001 | `luaL_openlibs()` loads ALL Lua stdlib including `os.execute()`, `io.open()`, `package.loadlib()` | **CRITICAL** | `GsKit/base/GsLua.cpp:52` | **FIXED** |
| LUA-002 | Base library functions `dofile`, `loadfile`, `load` allow arbitrary code loading | **CRITICAL** | `GsKit/base/GsLua.cpp` | **FIXED** |

---

## 3. Memory Safety — Buffer Overflows

| ID | Risk | Severity | File | Status |
|----|------|----------|------|--------|
| MEM-001 | VLA stack allocation with untrusted size from file data | **CRITICAL** | `src/fileio/CExeFile.cpp:525` | **FIXED** |
| MEM-002 | Unbounded patch write operations in CPatcher | HIGH | `src/fileio/CPatcher.cpp` | In Progress |
| MEM-003 | Integer overflow in depklite decompression | HIGH | `src/engine/depklite/depklite.cpp:267-289` | In Progress |
| MEM-004 | OOB write in Carmack decompression | HIGH | `src/fileio/compression/CCarmack.cpp` | In Progress |
| MEM-005 | OOB write in LZEXE decompression | HIGH | `src/fileio/compression/Cunlzexe.cpp` | In Progress |
| MEM-006 | Unchecked memcpy in audio unpacking | MEDIUM | `src/fileio/CExeFile.cpp:523` | Open |

---

## 4. Path Traversal

| ID | Risk | Severity | File | Status |
|----|------|----------|------|--------|
| PATH-001 | Patch file paths not validated — `../` traversal possible | HIGH | `src/fileio/CPatcher.cpp:80-99` | In Progress |
| PATH-002 | ZIP extraction — path traversal in archive filenames | MEDIUM | `src/engine/unzip/` | Open |

---

## 5. Unsafe C Function Usage

**Total instances found: 145**

| Severity | Count | Fixed | Remaining |
|----------|-------|-------|-----------|
| CRITICAL | 1 | 0 | 1 |
| HIGH | 1 | 0 | 1 |
| MEDIUM | 29 | ~14 | ~15 |
| LOW | 114 | 0 | 114 |

### CRITICAL Findings

| ID | File | Line | Function | Description | Status |
|----|------|------|----------|-------------|--------|
| SEC-001 | `src/engine/unzip/miniunz.c` | 375 | `scanf` | Unbounded `scanf` in archive extraction. Vendored code. | Open (vendored) |

### HIGH Findings

| ID | File | Line | Function | Description | Status |
|----|------|------|----------|-------------|--------|
| SEC-002 | `src/fileio/CPatcherUtils.cpp` | 144 | `sscanf` | Parses hex values from untrusted patch files | In Progress |

### MEDIUM Findings — By Category

#### File Parsing (Configuration, Fonts, Save Games)

| ID | File | Line | Function | Description | Status |
|----|------|------|----------|-------------|--------|
| SEC-003 | `GsKit/graphics/GsFont.cpp` | 65, 99 | `sscanf` | Font metadata and color parsing | Open |
| SEC-004 | `GsKit/graphics/GsFontLegacy.cpp` | 65, 99 | `sscanf` | Legacy font metadata and color parsing | Open |
| SEC-005 | `src/engine/core/CSettings.cpp` | 252 | `sscanf` | Aspect ratio parsing from config | In Progress |
| SEC-006 | `src/fileio/CSaveGameController.cpp` | 138, 262 | `atoi` | Save game file parsing (2 instances) | In Progress |
| SEC-007 | `src/fileio/CPatcher.cpp` | 73 | `atoi` | Patch file validation | In Progress |

#### UI / Menu Code

| ID | File | Line | Function | Description | Status |
|----|------|------|----------|-------------|--------|
| SEC-008 | `src/engine/core/menu/DisplaySettings.cpp` | 198-260 | `sscanf` | Display resolution/aspect parsing (3 instances) | In Progress |
| SEC-009 | `src/engine/core/menu/GameSpecSettings.cpp` | 102 | `sscanf` | Game resolution parsing | In Progress |
| SEC-010 | `src/engine/core/menu/AudioSettings.cpp` | 131 | `atoi` | Audio frequency parsing | In Progress |

#### RefKeen Integration

| ID | File | Line | Function | Description | Status |
|----|------|------|----------|-------------|--------|
| SEC-011 | `src/engine/refkeen/be_st_sdl.cpp` | 427-595 | `sscanf` | Configuration parsing (4 instances) | Open (vendored) |
| SEC-012 | `src/engine/keen/dreams/dreamscontrolpanel.cpp` | 230, 309 | `strcpy` | Dream menu string handling | Open |
| SEC-013 | `src/engine/refkeen/kdreams/id_us.cpp` | multiple | `strcpy` | 16 instances in menus/UI dialogs | Open (vendored) |

#### Command-Line Parsing

| ID | File | Line | Function | Description | Status |
|----|------|------|----------|-------------|--------|
| SEC-014 | `src/engine/keen/galaxy/GalaxyEngine.cpp` | 423 | `atoi` | Command-line level argument | In Progress |
| SEC-015 | `src/engine/keen/vorticon/VorticonEngine.cpp` | 241 | `atoi` | Command-line level argument | In Progress |

### LOW Findings Summary

| Category | Count | Notes |
|----------|-------|-------|
| Vendored Lua library | ~40 | Internal to Lua VM, bounded contexts |
| `vsprintf` in logging | 3 | **FIXED** — replaced with `vsnprintf` |
| `memcpy` operations | 68 | Graphics/audio/compression with proper sizes |
| Debug/crash handler code | ~3 | Only executes on crash, not user-facing |

---

## 6. File I/O Risks

| ID | Risk | Severity | File | Status |
|----|------|----------|------|--------|
| FIO-001 | EXE parser processes untrusted DOS executables — VLA overflow | **CRITICAL** | `src/fileio/CExeFile.cpp` | **FIXED** |
| FIO-002 | Save game decompression marked "still to be done" — incomplete | HIGH | `src/fileio/CSaveGameController.cpp:422` | Open |
| FIO-003 | Save game compression marked "still to be done" — incomplete | HIGH | `src/fileio/CSaveGameController.cpp:494` | Open |
| FIO-004 | ZIP extraction — zip bomb / path traversal potential | MEDIUM | `src/engine/unzip/` | Open |
| FIO-005 | LZ compression/decompression — malformed input handling | HIGH | `src/fileio/compression/` | In Progress |
| FIO-006 | depklite EXE depacker — malformed input | HIGH | `src/engine/depklite/` | In Progress |

---

## 7. Dependency Risks

| ID | Risk | Severity | Status |
|----|------|----------|--------|
| DEP-001 | SDL2 stack has no minimum version enforcement beyond "2.0+" | LOW | Open |
| DEP-002 | iOS FetchContent pins specific versions — need CVE audit for SDL2 2.30.10, curl 8.11.1 | MEDIUM | Open |
| DEP-003 | Vendored Lua version unknown — need to identify and check for CVEs | MEDIUM | Open |
| DEP-004 | Vendored minizip/unzip code — age and CVE status unknown | MEDIUM | Open |

---

## 8. Stability Risks

| ID | Risk | Severity | File | Status |
|----|------|----------|------|--------|
| STAB-001 | Known crash bug in Vorticon finale code | HIGH | `src/engine/keen/vorticon/finale/finale.cpp:98` | Open |
| STAB-002 | RefKeen audio/timer integration described as fundamentally misplaced | MEDIUM | `src/engine/refkeen/be_st_sdl_audio_timer.cpp:405` | Open |
| STAB-003 | Text mode rendering broken in RefKeen graphics | MEDIUM | `src/engine/refkeen/be_st_sdl_graphics.cpp:1533` | Open |
| STAB-004 | 20+ FIXME comments in RefKeen SDL integration | MEDIUM | `src/engine/refkeen/be_st_sdl.cpp` | Open |

---

## Summary

| Category | Critical | High | Medium | Low | Total |
|----------|----------|------|--------|-----|-------|
| Network Security | 5 (5 fixed) | 1 | 2 | 1 | 9 |
| Scripting (Lua) | 2 (2 fixed) | 0 | 0 | 0 | 2 |
| Memory Safety | 1 (1 fixed) | 4 | 1 | 0 | 6 |
| Path Traversal | 0 | 1 | 1 | 0 | 2 |
| Unsafe C Functions | 1 | 1 | 29 | 114 | 145 |
| File I/O | 1 (1 fixed) | 4 | 1 | 0 | 6 |
| Dependencies | 0 | 0 | 3 | 1 | 4 |
| Stability | 0 | 1 | 3 | 0 | 4 |
| **Total** | **10 (9 fixed)** | **12** | **40** | **116** | **178** |

### Remediation Progress

| Status | Count |
|--------|-------|
| **FIXED** | 9 |
| In Progress | ~20 |
| Open | ~149 |
| Open (vendored — won't fix) | ~60 |
