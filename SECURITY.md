# Security Policy

## Supported Versions

| Version | Supported |
|---|---|
| 0.9.x (current) | ✅ |
| 0.7.x – 0.8.x | ⚠️ Best-effort |
| < 0.7 | ❌ No longer supported |

## Scope

AdEmpire is a local single-player desktop game. The attack surface is limited to:

- **Save file parsing** — `nlohmann/json` deserializes `saves/*.json` from the user's own machine
- **Event data parsing** — `assets/data/events.json` loaded at startup
- **REST leaderboard** (v0.8+, optional) — HTTP POST to a remote endpoint if enabled

There is no authentication, no user accounts, and no network-accessible server bundled with the game.

## Reporting a Vulnerability

If you discover a security issue (e.g., a malformed save file causing a buffer overrun, or a path traversal in event loading), please **do not** open a public issue.

Instead, open a [GitHub Security Advisory](https://github.com/Gzeu/AdEmpire/security/advisories/new) (private disclosure) or email the maintainer directly via GitHub profile.

We aim to respond within **72 hours** and to publish a patch within **7 days** for confirmed vulnerabilities.

## Third-Party Dependencies

| Dependency | Source | Notes |
|---|---|---|
| Dear ImGui | git submodule | Rendering only, no network |
| GLFW 3 | System package / vcpkg | Windowing only |
| nlohmann/json | FetchContent (CMake) | JSON parse — validate input |
| cpp-httplib | Header-only, optional | REST leaderboard only |
| OpenAL-soft | System package, optional | Audio only |

Keep dependencies up to date via your system package manager or vcpkg.
