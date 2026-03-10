# Commander Genius — Validation Workplan

**Status:** Phase 2 In Progress
**Created:** 2026-03-07
**Purpose:** Systematic validation of the Commander Genius codebase acquired from the original development team, ensuring it meets production standards for deployment across all supported platforms.

## Context

Commander Genius is a ~143k-line C++ game engine (586 source files + 240 GsKit framework files) that interprets classic Commander Keen games. It targets Linux, macOS, Windows, Android, iOS, and Nintendo Switch, with legacy support for embedded platforms (GP2X, Wiz, Caanoo, Dingoo, NanoNote — to be deprecated).

The project was acquired from a company that is no longer operating. The current team has done some exploration but no substantive changes. This workplan establishes a sequential, gated process to bring the project to deployment-ready status.

### Key Findings from Initial Assessment

- **No test suite** — zero unit tests, no test framework integrated
- **No code quality tooling** — no `.clang-format`, `.editorconfig`, or linters
- **145 instances** of unsafe C functions (`strcpy`, `sprintf`, `scanf`, `atoi`, etc.) — 1 critical, 1 high, 29 medium, 114 low
- **Network code** (curl-based game downloader) with no download integrity verification
- **422 compiler warnings** on macOS (Apple Clang 17) — mostly invalid source encoding (244), deprecated declarations (96), missing `override` (31)
- **No existing CI/CD** — repo is being moved; pipelines will be built from scratch
- **License:** GPL v2

### How to Use This Workplan

Each phase is sequential — complete all tasks and pass the gate review before moving to the next phase. Within each phase, tasks without dependencies on each other can be parallelized.

Mark tasks with:
- `[ ]` — not started
- `[~]` — in progress
- `[x]` — complete
- `[!]` — blocked (add a note explaining why)

---

## Phase 0: Inventory & Triage

**Objective:** Establish a complete picture of what we've acquired — what builds, what's broken, what's risky.

### Tasks

- [x] **0.1** Catalog all source files with rough purpose mapping
  - `src/engine/core/` — core engine (game loop, camera, collision, HUD, menus)
  - `src/engine/keen/vorticon/` — Keen 1-3 engine
  - `src/engine/keen/galaxy/` — Keen 4-6 engine
  - `src/engine/keen/dreams/` — Keen Dreams (CG wrapper around RefKeen)
  - `src/engine/cosmos/` — Cosmos Cosmic Adventure (INCOMPLETE — stub only, git submodule)
  - `src/fileio/` — file I/O, EXE parsing, save games, compression
  - `src/dialog/` — dialog boxes and text viewers
  - `src/engine/gamedownloader.*` — network game catalogue
  - `GsKit/` — shared framework (55,700 LOC: graphics, input, audio, widgets, Lua)
  - See `docs/validation/inventory-report.md` for full directory map
- [x] **0.2** Attempt builds on primary platforms — document results
  - [!] Linux (GCC 14) — no local Linux environment; deferred to CI setup
  - [!] Linux (Clang) — no local Linux environment; deferred to CI setup
  - [x] macOS (Apple Clang 17) — **BUILD SUCCESS** (4.4MB arm64 binary, 422 warnings)
  - [!] Windows (MSYS2) — no local Windows environment; deferred to CI setup
- [x] **0.3** Inventory all third-party dependencies and their versions
  - SDL2 2.0+ (soft), SDL2_image 2.0+, SDL2_mixer 2.0+, SDL2_ttf 2.0+
  - curl >= 7.32 (hard check), zlib (any), OpenGL (optional), Boost (optional, default OFF)
  - Vendored: Lua 5.3+, RefKeen, minizip, depklite, tinyxml2, dbopl
  - iOS pins: SDL2 2.30.10, SDL2_image 2.8.2, SDL2_mixer 2.8.1, SDL2_ttf 2.24.0, curl 8.11.1
  - See `docs/validation/dependency-matrix.md` for full matrix
- [ ] **0.4** Audit dependency versions against known CVEs
  - Vendored Lua, minizip, tinyxml2 versions need identification and CVE check
  - iOS FetchContent pins need CVE audit
- [x] **0.5** Map all 145 unsafe C function usages — classify by severity
  - **Critical (1):** `scanf` in `src/engine/unzip/miniunz.c:375`
  - **High (1):** `sscanf` in `src/fileio/CPatcherUtils.cpp:144`
  - **Medium (29):** config parsing, font loading, save games, command-line args
  - **Low (114):** vendored Lua library, logging, memcpy with proper sizes
  - See `docs/validation/risk-register.md` for full breakdown
- [x] **0.6** Identify dead code paths and legacy platform code
  - Legacy platforms (GP2X, Wiz, Caanoo, Dingoo, NanoNote, Pandora) cleanly separated behind `#ifdef` guards
  - `src/engine/refkeen/kdreams/id_vw_ac.cpp` — entire file disabled with `#if 0`
  - BUILD_COSMOS — infrastructure stub only, no actual engine implementation
  - 150+ TODO/FIXME/HACK markers across codebase
  - Known crash bug: `src/engine/keen/vorticon/finale/finale.cpp:98`
  - Decision: **Legacy platforms formally deprecated**
- [x] **0.7** Catalog existing CI pipelines — **NOT APPLICABLE**
  - Repo is being moved to new hosting; existing GitLab CI and GitHub Actions will not carry over
  - CI/CD will be built from scratch in Phase 6
- [x] **0.8** Document the GsKit framework boundary
  - GsKit is **directly integrated** (NOT a submodule), shares repo and commit history
  - Provides: input, rendering, audio, UI widgets, events, logging, Lua scripting, config, timers
  - Builds as 8 static libraries (GsKit_base, GsKit_base_interface, etc.)
  - Only submodule: `src/engine/cosmos` (external gitlab repo, incomplete)
  - See `docs/validation/inventory-report.md` for full analysis

### Gate Criteria

- [x] Build status documented for macOS; Linux/Windows deferred to CI setup (no local environments)
- [x] Dependency inventory complete with version numbers — see `docs/validation/dependency-matrix.md`
- [x] Risk register created (145 unsafe functions, network code, file I/O, stability) — see `docs/validation/risk-register.md`
- [x] Decision logged: legacy platforms (GP2X, Wiz, Caanoo, Dingoo, NanoNote, Pandora) formally deprecated

### Deliverables

- `docs/validation/inventory-report.md`
- `docs/validation/risk-register.md`
- `docs/validation/dependency-matrix.md`

---

## Phase 1: Build System Validation

**Objective:** Confirm the project compiles cleanly on all supported platforms with all meaningful CMake configuration variants.

### Tasks

- [x] **1.1** Fix any build failures discovered in Phase 0
  - Fixed `BUILD_REFKEEN=OFF` linker failure (guarded Dreams code in CGameLauncher.cpp)
  - Fixed `xxd.c` missing function prototypes
  - Fixed `GsLogging.cpp` vsprintf → vsnprintf
- [x] **1.2** Validate CMake option combinations (all on macOS)
  - [x] `BUILD_KEEN=ON` + `BUILD_REFKEEN=ON` (default) — PASS
  - [x] `BUILD_KEEN=ON` + `BUILD_REFKEEN=OFF` — PASS (after fix)
  - [ ] `BUILD_COSMOS=ON` — cmake passes, build not tested (incomplete feature)
  - [x] `DOWNLOADER=ON` vs `DOWNLOADER=OFF` — PASS
  - [x] `USE_VIRTUALPAD=ON` vs `USE_VIRTUALPAD=OFF` — PASS
  - [x] `USE_OPENGL=ON` vs `USE_OPENGL=OFF` — PASS
- [~] **1.3** Build on each primary platform
  - [!] Linux — GCC 14 — deferred to CI setup
  - [!] Linux — Clang (latest) — deferred to CI setup
  - [x] macOS — Apple Clang 17.0.0 — PASS (4.4MB arm64 binary)
  - [!] Windows — MSYS2 (MinGW-w64) — deferred to CI setup
- [!] **1.4** Cross-compile for secondary platforms — deferred to CI setup
  - [!] Android (NDK)
  - [!] iOS (Xcode toolchain)
  - [!] Nintendo Switch (devkit toolchain)
- [x] **1.5** Resolve all compiler warnings — **422 → 3** (99.3% reduction)
  - [x] Suppressed 244 encoding warnings in vendored RefKeen (CP437 intentional)
  - [x] Suppressed 96 deprecated-declarations in vendored RefKeen
  - [x] Suppressed 12 string-plus-int in vendored RefKeen
  - [x] Suppressed 3 deprecated-non-prototype in vendored minizip/xxd
  - [x] Fixed 32 unqualified `std::move()` calls across 11 files
  - [x] Added `override` keyword to 29+ virtual function overrides across 29 files
  - [x] Fixed `vsprintf` → `vsnprintf` in GsLogging.cpp (3 instances)
  - [x] Fixed `xxd.c` missing function prototypes
  - Remaining 3: empty libGsKit.a (meta-target), duplicate libraries (circular CMake deps), `#warning` (intentional)
- [x] **1.6** Validate dependency resolution on macOS
  - SDL2 2.32.10, SDL2_image 2.8.8, SDL2_mixer 2.8.1, SDL2_ttf 2.24.0 (Homebrew)
  - curl 8.7.1 (system), zlib 1.2.12 (system)
- [!] **1.7** Test packaging targets — deferred to CI setup
  - [!] DEB (Linux)
  - [!] RPM (Linux)
  - [ ] DMG (macOS) — could test locally
  - [!] NSIS installer (Windows)
  - [!] Portable ZIP (Windows)
- [x] **1.8** Document minimum compiler and OS versions per platform
  - See `docs/validation/build-matrix.md`

### Gate Criteria

- [x] Clean builds on macOS (3 warnings remaining — all non-source-code). Linux/Windows deferred to CI.
- [!] Cross-compilation deferred to CI setup
- [!] Packaging deferred to CI setup (DMG testable locally)
- [x] All CMake option combinations build without error (on macOS)

### Deliverables

- `docs/validation/build-matrix.md` — platform x config results table
- Build fix commits merged to repo
- Updated `README.md` build instructions if any steps changed

---

## Phase 2: Security Audit & Remediation

**Objective:** Identify and fix security vulnerabilities, with emphasis on network-facing code and unsafe memory operations.

### Tasks

#### 2a. Static Analysis

- [x] **2.1** Integrate static analyzers into the build
  - [x] clang-tidy with security-relevant checks (`.clang-tidy` committed)
  - [ ] cppcheck for undefined behavior and memory issues
- [~] **2.2** Run full scan — catalog all findings by severity
- [~] **2.3** Fix all critical and high findings
- [ ] **2.4** Document accepted risks for medium/low findings

#### 2b. Unsafe Function Remediation

- [ ] **2.5** Replace all `strcpy` with `strncpy` or `std::string`
- [x] **2.6** Replace all `sprintf` with `snprintf` or `std::format`/stream formatting (vsprintf→vsnprintf in GsLogging.cpp)
- [ ] **2.7** Replace all `gets` with `fgets` or C++ stream equivalents
- [~] **2.8** Replace all unbounded `scanf` with bounded variants or C++ parsing (14 files in progress)
- [~] **2.9** Audit remaining raw pointer arithmetic for buffer overflow potential

#### 2c. Network Security

- [x] **2.10** Audit `gamedownloader.cpp` — validate URLs, check for SSRF
- [x] **2.11** Enforce HTTPS-only for all network requests (no HTTP fallbacks)
- [ ] **2.12** Add download integrity verification (checksum validation)
- [x] **2.13** Audit curl options for safe defaults (cert verification, redirect limits, timeouts)

#### 2d. File I/O Security

- [x] **2.14** Audit `CExeFile` EXE parser for buffer overflows and malformed input (VLA→vector fix)
- [ ] **2.15** Audit `CSaveGameController` deserialization for injection/overflow
- [~] **2.16** Audit compression/decompression (LZ, PKWARE) for zip bomb/overflow
- [~] **2.17** Validate all file path construction against path traversal attacks

#### 2e. Input Handling

- [ ] **2.18** Audit command-line argument parsing in `CGeniusEntry.cpp`
- [ ] **2.19** Audit configuration file parsing for injection
- [x] **2.20** Review Lua scripting sandbox — ensure scripts can't escape to host filesystem (sandboxed)

### Gate Criteria

- [~] Zero critical/high static analysis findings (7/10 CRITICAL fixed, HIGH in progress)
- [~] All unsafe C function usages resolved or explicitly risk-accepted with justification
- [x] Network code enforces HTTPS with certificate verification
- [~] File parsers validated against malformed input
- [x] Lua sandbox reviewed and hardened
- [x] All findings documented in risk register

### Deliverables

- All remediation patches committed
- `docs/validation/security-audit-report.md`
- Updated `docs/validation/risk-register.md`
- Static analysis configuration files committed to repo (`.clang-tidy`)

---

## Phase 3: Code Quality Baseline

**Objective:** Establish enforceable coding standards and bring the codebase to a consistent, maintainable state.

### Tasks

#### 3a. Tooling Setup

- [ ] **3.1** Create `.clang-format` config matching existing dominant code style
- [ ] **3.2** Create `.clang-tidy` config with curated modernization rules
  - `modernize-*`, `readability-*`, `performance-*`
- [ ] **3.3** Create `.editorconfig` for consistent whitespace/encoding
- [ ] **3.4** Add pre-commit hook or script for formatting checks

#### 3b. Incremental Formatting

- [ ] **3.5** Apply `clang-format` module-by-module (separate commits to preserve git blame)
  - [ ] `GsKit/`
  - [ ] `src/fileio/`
  - [ ] `src/dialog/`
  - [ ] `src/engine/core/`
  - [ ] `src/engine/keen/vorticon/`
  - [ ] `src/engine/keen/galaxy/`
  - [ ] `src/engine/keen/dreams/`
  - [ ] `src/engine/cosmos/`
  - [ ] `src/engine/refkeen/`

#### 3c. Modernization (Low-Risk)

- [ ] **3.6** Replace raw `new`/`delete` with smart pointers where straightforward
- [ ] **3.7** Replace C-style casts with `static_cast`/`reinterpret_cast`
- [ ] **3.8** Apply `auto` type deduction where it improves readability
- [ ] **3.9** Replace `NULL` with `nullptr` throughout
- [ ] **3.10** Add `override` keyword to all overriding virtual functions

#### 3d. Dead Code & Deprecation

- [ ] **3.11** Add deprecation markers to legacy platform code (GP2X, Wiz, Caanoo, Dingoo, NanoNote)
- [ ] **3.12** Remove confirmed dead code paths from Phase 0
- [ ] **3.13** Clean up unused `#include` directives

### Gate Criteria

- [ ] `.clang-format`, `.clang-tidy`, `.editorconfig` committed
- [ ] Full codebase formatted (one commit per module)
- [ ] `clang-tidy` passes with zero warnings on chosen rule set
- [ ] Legacy platform code marked deprecated
- [ ] No regressions — all Phase 1 builds still pass

### Deliverables

- Code quality tooling configs in repo root
- Formatting commits (per-module)
- Modernization commits (grouped by change type)
- `docs/validation/code-quality-report.md`

---

## Phase 4: Test Infrastructure & Test Suites

**Objective:** Stand up a test framework from scratch and build meaningful coverage across critical subsystems.

### Tasks

#### 4a. Test Infrastructure

- [ ] **4.1** Integrate Google Test into the CMake build
- [ ] **4.2** Create `tests/` directory structure mirroring `src/` layout
- [ ] **4.3** Add CMake test targets with `ctest` integration
- [ ] **4.4** Add test stage to CI pipelines (GitLab CI + GitHub Actions)
- [ ] **4.5** Set up code coverage reporting (gcov/lcov or llvm-cov)

#### 4b. Unit Tests — Core Engine

- [ ] **4.6** `CMap` — map loading, tile access, bounds checking
- [ ] **4.7** `CSpriteObject` — sprite positioning, state transitions
- [ ] **4.8** `CObjectCollision` — collision detection correctness
- [ ] **4.9** `CCamera` — camera tracking, boundary clamping
- [ ] **4.10** `CBehaviorEngine` — behavior state machine transitions
- [ ] **4.11** `scrollingplane` — parallax calculation correctness

#### 4c. Unit Tests — File I/O

- [ ] **4.12** `CExeFile` — valid EXE parsing, malformed input rejection
- [ ] **4.13** `CSaveGameController` — save/load round-trip integrity, corrupt data handling
- [ ] **4.14** `CTileLoader` — tile data extraction correctness
- [ ] **4.15** `CPatcher` — patch application and validation
- [ ] **4.16** Compression/decompression — round-trip correctness, malformed input
- [ ] **4.17** CRC — known-value verification

#### 4d. Unit Tests — Network & Downloads

- [ ] **4.18** `gamedownloader` — URL validation, error handling (mock curl responses)
- [ ] **4.19** Catalogue parsing — valid and malformed catalogue data

#### 4e. Unit Tests — GsKit Framework

- [ ] **4.20** Graphics primitives — surface creation, blitting
- [ ] **4.21** Widget system — input handling, state management
- [ ] **4.22** Lua scripting bridge — script execution, sandbox boundary tests

#### 4f. Integration Tests

- [ ] **4.23** Game launcher — startup sequence without game data
- [ ] **4.24** Configuration — settings save/load round-trip
- [ ] **4.25** Platform smoke tests — launch and exit cleanly on each primary platform

#### 4g. Coverage & Quality

- [ ] **4.26** Set initial coverage target: 40% overall, 70%+ on file I/O and network code
- [ ] **4.27** Configure CI to fail if coverage drops below threshold
- [ ] **4.28** Add coverage badge to README

### Gate Criteria

- [ ] Google Test integrated, `ctest` runs from build directory
- [ ] Minimum 40% overall code coverage
- [ ] 70%+ coverage on file I/O and network subsystems
- [ ] All tests pass on Linux, macOS, Windows
- [ ] CI runs tests on every push
- [ ] No Phase 1 build regressions

### Deliverables

- `tests/` directory with full test suite
- Coverage reports in CI artifacts
- `docs/validation/test-coverage-report.md`

---

## Phase 5: Developer Documentation Handbook

**Objective:** Produce a complete developer handbook so a new contributor can build, understand, and contribute within a day.

### Tasks

#### 5a. Onboarding Guide

- [ ] **5.1** Rewrite `README.md` — streamline and modernize build instructions
- [ ] **5.2** Create `docs/getting-started.md` — clone, build, run, first change walkthrough
- [ ] **5.3** Create `docs/dev-environment.md` — recommended IDEs, debugger setup, platform tooling
- [ ] **5.4** Create `CONTRIBUTING.md` — contribution workflow, PR process, coding standards

#### 5b. Architecture Reference

- [ ] **5.5** Create `docs/architecture/overview.md` — high-level system diagram, subsystem map
- [ ] **5.6** Create `docs/architecture/engine-core.md` — game loop, scene/mode system, event flow
- [ ] **5.7** Create `docs/architecture/keen-engines.md` — Vorticon vs Galaxy vs Dreams differences
- [ ] **5.8** Create `docs/architecture/gskit.md` — framework boundary, what it provides
- [ ] **5.9** Create `docs/architecture/rendering.md` — SDL2 pipeline, OpenGL path, sprites
- [ ] **5.10** Create `docs/architecture/audio.md` — SDL2_mixer integration, music/SFX
- [ ] **5.11** Create `docs/architecture/networking.md` — game downloader, catalogue, security model

#### 5c. Platform Guides

- [ ] **5.12** Create `docs/platforms/linux.md` — build, package, distribution
- [ ] **5.13** Create `docs/platforms/macos.md` — osxcross, DMG, codesigning
- [ ] **5.14** Create `docs/platforms/windows.md` — MSYS2, NSIS installer, DLLs
- [ ] **5.15** Create `docs/platforms/android.md` — NDK, JNI layer, APK build
- [ ] **5.16** Create `docs/platforms/ios.md` — Xcode, FetchContent deps, App Store notes
- [ ] **5.17** Create `docs/platforms/switch.md` — devkit requirements, build process
- [ ] **5.18** Create `docs/platforms/legacy.md` — deprecated platforms, status, removal timeline

#### 5d. Coding Standards & Conventions

- [ ] **5.19** Create `docs/coding-standards.md` — formatting, naming, patterns to follow/avoid
- [ ] **5.20** Create `docs/testing-guide.md` — how to write/run tests, coverage targets
- [ ] **5.21** Create `docs/security-guidelines.md` — safe patterns, review checklist

#### 5e. API & Code Reference

- [ ] **5.22** Update and validate Doxyfile — ensure it generates current docs
- [ ] **5.23** Run Doxygen, publish output, link from handbook
- [ ] **5.24** Add inline documentation to key public interfaces in GsKit

### Gate Criteria

- [ ] New developer can follow getting-started guide and build within 1 hour
- [ ] All architecture docs reviewed by at least one team member
- [ ] Doxygen generates without warnings
- [ ] All platform guides validated by attempting steps on each platform
- [ ] `CONTRIBUTING.md` merged to repo

### Deliverables

- Complete `docs/` directory structure as listed above
- Updated `README.md`
- `CONTRIBUTING.md` in repo root
- Generated Doxygen API reference

---

## Phase 6: CI/CD Hardening & Release Validation

**Objective:** Make CI/CD pipelines robust, comprehensive, and trustworthy for production releases.

### Tasks

#### 6a. Pipeline Consolidation

- [ ] **6.1** Unify GitLab CI and GitHub Actions — both run the same validation steps
- [ ] **6.2** Add build matrix: all primary platforms x meaningful CMake option combos
- [ ] **6.3** Add test stage to both pipelines
- [ ] **6.4** Add static analysis stage (clang-tidy, cppcheck)
- [ ] **6.5** Add code coverage reporting stage with threshold enforcement

#### 6b. Release Pipeline

- [ ] **6.6** Add automated version tagging from `version.h`
- [ ] **6.7** Validate all packaging formats build correctly in CI
- [ ] **6.8** Add artifact signing for release builds where applicable
- [ ] **6.9** Add changelog generation from git history

#### 6c. Quality Gates

- [ ] **6.10** Enforce: build must pass on all platforms before merge
- [ ] **6.11** Enforce: all tests must pass before merge
- [ ] **6.12** Enforce: no new clang-tidy warnings before merge
- [ ] **6.13** Enforce: code coverage cannot drop below threshold
- [ ] **6.14** Document branch protection rules

### Gate Criteria

- [ ] CI runs build + test + analysis on every push to main
- [ ] All packaging formats produce valid artifacts in CI
- [ ] Quality gates enforced — broken builds block merges
- [ ] Pipeline architecture documented

### Deliverables

- Updated `.gitlab-ci.yml` and `.github/workflows/`
- `docs/ci-cd.md` — pipeline architecture, how to add new targets
- Release process documentation

---

## Phase 7: Final Gate Review

**Objective:** Comprehensive validation that all phases delivered their outcomes and the project is deployment-ready.

### Tasks

- [ ] **7.1** Review all gate criteria from Phases 0–6 — confirm each is met
- [ ] **7.2** Run full build + test suite on all primary platforms
- [ ] **7.3** Run full security scan with updated tools
- [ ] **7.4** Smoke test end-to-end on each platform
  - Launch game → load game data → play → save → exit
  - [ ] Linux
  - [ ] macOS
  - [ ] Windows
  - [ ] Android
  - [ ] iOS
- [ ] **7.5** Review all documentation for accuracy against current codebase
- [ ] **7.6** Create deployment readiness checklist
- [ ] **7.7** Conduct team walkthrough of workplan results

### Gate Criteria

- [ ] All prior phase gates confirmed passing
- [ ] End-to-end smoke test passes on all primary platforms
- [ ] No open critical/high security findings
- [ ] Documentation reviewed and current
- [ ] Team sign-off on deployment readiness

### Deliverables

- `docs/validation/final-gate-review.md`
- `docs/validation/deployment-readiness.md`
- This workplan with all checkboxes completed

---

## Summary

| Phase | Objective | Key Metric |
|-------|-----------|------------|
| 0 | Inventory & Triage | Risk register complete |
| 1 | Build System Validation | Clean builds on all platforms |
| 2 | Security Audit & Remediation | Zero critical/high findings |
| 3 | Code Quality Baseline | clang-tidy/format enforced |
| 4 | Test Infrastructure & Suites | 40% coverage, 70%+ on I/O & network |
| 5 | Developer Documentation | New dev productive in 1 hour |
| 6 | CI/CD Hardening | Quality gates enforced on all merges |
| 7 | Final Gate Review | Team sign-off on deployment readiness |
