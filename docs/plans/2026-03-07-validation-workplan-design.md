# Validation Workplan — Design Document

**Date:** 2026-03-07
**Status:** Approved

## Problem Statement

Commander Genius was acquired from a company that is no longer operating. The codebase (~143k lines of C++, GPL v2) has no test suite, no code quality tooling, 215 unsafe C function usages, minimal CI, and network code without integrity verification. The team has limited familiarity with the codebase.

We need a structured, sequential process to validate and harden this project for confident deployment across all supported platforms (Linux, macOS, Windows, Android, iOS, Nintendo Switch).

## Approach Selected

**Layered Confidence Building** — sequential phases with gate reviews, each building on the validated foundation of the previous phase. Sub-tasks within phases can be parallelized.

### Alternatives Considered

1. **Linear Waterfall** — simple but slow, blocks parallel contributors
2. **Risk-Driven Sprints** — addresses scariest issues first but harder to track, no clear milestones

### Why Layered Confidence Building

- Clear milestones and gate reviews
- Each phase produces a deliverable
- Sequential layering means each phase builds on validated work
- Gate reviews prevent wasted work (critical for a team new to the codebase)

## Key Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Platform support | All original platforms | Maintain full compatibility per acquisition goals |
| Legacy platforms (GP2X, etc.) | Deprecate gracefully | Leave code, mark deprecated, document in legacy guide |
| Test framework | Google Test | Mature, CMake-friendly, well-suited for C++ |
| Coverage target | 40% overall, 70% I/O/network | Pragmatic — highest confidence where risk is highest |
| Documentation level | Full handbook | New team needs comprehensive onboarding + architecture reference |
| Formatting strategy | Module-by-module commits | Preserves git blame history |

## Phase Structure

8 phases (0-7), each with: objective, tasks, gate criteria, deliverables.

Full workplan: [`VALIDATION_WORKPLAN.md`](../../VALIDATION_WORKPLAN.md)

## Risks

| Risk | Mitigation |
|------|------------|
| GsKit has external dependencies we don't control | Phase 0 documents the boundary; consider vendoring |
| Legacy platform code may have tentacles throughout | Phase 0 maps dependencies before Phase 3 deprecation |
| 143k lines is a lot to format at once | Module-by-module approach in Phase 3 |
| No test infrastructure means coverage ramp is slow | Start with highest-risk subsystems (I/O, network) |
| Team unfamiliarity may cause incorrect assumptions | Gate reviews at every phase boundary |
