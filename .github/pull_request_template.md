## Summary
<!-- One sentence: what does this PR do? -->

## Related Issue
Closes #

## Changes
- 
- 

## Systems Affected
- [ ] Campaign
- [ ] Client / Negotiation
- [ ] Staff
- [ ] AI Competitors
- [ ] Events (JSON)
- [ ] UI / ImGui
- [ ] Audio
- [ ] Build / CMake
- [ ] Docs

## Checklist
- [ ] Compiles with `cmake .. && make -j$(nproc)` — zero errors
- [ ] No `new`/`delete` — value semantics throughout
- [ ] New moddable data goes in `assets/data/` not hardcoded
- [ ] If new `.cpp` file added, it's listed in `CMakeLists.txt`
- [ ] CHANGELOG.md updated under `[Unreleased]`
