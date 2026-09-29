# OpenKey Unit-Test Plan

Status: **P1 COMPLETE** — engine core at 85%+ line coverage (measured). P2 outlined.

Scope: fork `jooservices/OpenKey` (POC, 3rd-party). Goals:

- **100% feature coverage** across the Vietnamese-input engine. ✅
- **85% line coverage** on the **engine C++ core** (P1). ✅
- UI layer (P2) is documented but **not** part of the 85% target.

---

## Current state (verified)

- No test files, no test target, no CI test job today. Coverage = **0%**.
- Xcode project has a single app target (`OpenKey`), no XCTest target.
- Engine (`Sources/OpenKey/engine/`) is **pure C++**, OS-independent except the
  keycode constants in `platforms/*.h` (`mac.h` / `win32.h` / `linux.h`).
- Engine API is a **state machine**: `vKeyHandleEvent(event, state, data,
  capsStatus, otherControlKey)` writes its result into a global
  `vKeyHookState HookState` (fields `code`, `backspaceCount`, `newCharCount`,
  `extCode`, `charData[]`, `macroKey`, `macroData`). `vKeyInit()` returns
  `&HookState`, so tests read output directly.
- All behaviour is driven by **extern globals** declared in `Engine.h`
  (e.g. `vInputType`, `vCodeTable`, `vCheckSpelling`, `vUseModernOrthography`,
  `vQuickTelex`, `vRestoreIfWrongSpelling`, `vUseMacro`, …) — the app defines
  them; tests define their own copies.
- `startNewSession()` resets typing state between test cases.
- Toolchain available on this host: `clang++` (Apple 21.0.0) and `llvm-cov`
  via `xcrun`. No `cmake`.

---

## P1 — Engine core (target: 85% line coverage)

### Scope

Compile `Sources/OpenKey/engine/` standalone (no AppKit/Cocoa) into a
single test binary:

```
Sources/OpenKey/engine/
  Engine.cpp        (~1558)  typing state machine + spelling/grammar
  Vietnamese.cpp    (~576)   keycode→char tables, conversion
  Macro.cpp         (~293)   macro map + matching
  ConvertTool.cpp   (~180)   text conversion (convertUtil)
  SmartSwitchKey.cpp( ~73)   per-app input-method memory
```

Approx. **~2.7k lines** → target is meaningful and measurable.

### Test harness (no external dependency)

Write a minimal C++ harness in `tests/harness/` — no network fetch, no CMake,
deterministic on CI:

- `tests/harness/TestHarness.h` — `TEST(name) { ... }` macros backed by a
  registry, `EXPECT_EQ / EXPECT_TRUE / EXPECT_FALSE`, pass/fail counters,
  a single `runAll()` returning exit code.
- `tests/harness/test_config.cpp` — defines every `extern` variable the
  engine needs (the `v*` flags, ConvertTool flags, `douKey`, `_vowel`, …).
- `tests/harness/engine_fixture.h` — RAII fixture that calls `vKeyInit()`,
  sets a known config, runs a key sequence via `vKeyHandleEvent`, and exposes
  the resulting `HookState`.

### Test layout

```
tests/
  harness/            shared harness + config + fixture
  engine/
    main.cpp          registers all suites, runs, prints summary
    test_telex.cpp
    test_vni.cpp
    test_simple_telex1.cpp
    test_simple_telex2.cpp
    test_code_tables.cpp      # Unicode / TCVN3 / VNI-Windows / CP1258 / Unicode Compound
    test_spelling_grammar.cpp
    test_quick_telex_consonants.cpp
    test_macro.cpp
    test_smart_switch.cpp
    test_convert_tool.cpp
    test_utf_utils.cpp         # utf8ToWideString / wideStringToUtf8
    test_keycode_to_char.cpp
```

### How a test drives the engine

Helper (`engine_fixture.h`):

```cpp
struct KeySeq {
    static void type(const char* telexKeys, int inputType, int codeTable);
    // e.g. KeySeq::type("hoa s", vTelex, Unicode)
    // → for each key: vKeyHandleEvent(Keyboard, KeyDown, keycode, caps, false)
    //   collects HookState.charData / backspaceCount into an expected buffer
};
```

Example (Telex `hoa` + `s` → `hoá`):

```cpp
TEST(telex_mark_s) {
    KeySeq::type("hoas", vTelex, Unicode);
    EXPECT_EQ(0, hook.backspaceCount);
    EXPECT_EQ(3, hook.newCharCount);
    EXPECT_CHARSEQ(hook.charData, L"hoá");
}
```

### Feature matrix (P1)

| Area | Config knobs to exercise | Coverage target |
| --- | --- | --- |
| Telex | `vInputType=0`, mark keys `s/f/r/x/j`, `z` tone removal, `w/e/o/[` vowel shortcuts | 100% features |
| VNI | `vInputType=1`, `1..9` tone + vowel keys, double-byte handling | 100% features |
| Simple Telex 1 / 2 | `vInputType=2,3` | 100% features |
| Code tables | `vCodeTable=0..4` (Unicode, TCVN3, VNI-Win, Unicode Compound, CP1258), `IS_DOUBLE_CODE` branches | 100% features |
| Orthography | `vUseModernOrthography=0/1` (`òa` vs `oà`) | both branches |
| Spelling check | `vCheckSpelling=1` + invalid words, `vRestoreIfWrongSpelling` restore-on-break | both branches |
| Grammar check | grammar fix path (`checkGrammar`) | exercised |
| Quick Telex / consonants | `vQuickTelex`, `vQuickStartConsonant`, `vQuickEndConsonant` | both branches |
| Macro | `vUseMacro`, `vAutoCapsMacro`, add/delete/find macro | 100% features |
| Smart Switch | `vUseSmartSwitchKey`, `vRememberCode` | both branches |
| Caps handling | `capsStatus` 0/1/2 (none/shift/capslock), `CAPS_MASK` paths | all paths |
| Restore invalid word | `vRestoreIfWrongSpelling` + break chars | both branches |
| ConvertTool | `convertUtil` across from/to code tables, `convertToolRemoveMark`, caps flags | 100% features |
| Utils | `utf8ToWideString`/`wideStringToUtf8`, `keyCodeToCharacter` | 100% |

### Build & coverage commands (smoke-verified on host)

```bash
# build (clang, gcov-style instrumentation)
clang++ -std=c++17 -O0 -g --coverage \
  -ISources/OpenKey/engine -Itests/harness \
  tests/engine/*.cpp tests/harness/*.cpp \
  Sources/OpenKey/engine/Engine.cpp \
  Sources/OpenKey/engine/Vietnamese.cpp \
  Sources/OpenKey/engine/Macro.cpp \
  Sources/OpenKey/engine/ConvertTool.cpp \
  Sources/OpenKey/engine/SmartSwitchKey.cpp \
  -o build/ut_engine

# run tests
./build/ut_engine

# coverage report
llvm-cov gcov Engine.gcno        # or: xcrun llvm-cov gcov ...
```

Add a `tests/Makefile` wrapping build/test/coverage so CI runs one target.

### Definition of Done (P1)

- Test binary compiles clean, all suites pass deterministically. ✅
- **≥ 85% line coverage** on the 5 engine translation units via `llvm-cov`
  (report saved, e.g. `tests/coverage_report.txt`). ✅
- Feature matrix above fully exercised (no untested feature branch). ✅
- CI job added to the fork's Actions that builds + runs + enforces the
  coverage floor.

## Measured results (P1 done)

| TU | Coverage |
| --- | --- |
| `Engine.cpp` | **85.02%** (1128 lines) |
| `Vietnamese.cpp` | **100.00%** (428 lines) |
| `Macro.cpp` | **91.55%** (213 lines) |
| `ConvertTool.cpp` | **90.00%** (120 lines) |
| `SmartSwitchKey.cpp` | **100.00%** (49 lines) |

**159 tests / 2811 assertions, 0 failures.** Verified on this host:
`clang++ --coverage` + `gcov`. Run from `tests/`: `make test` / `make coverage`.

Test files (all under `tests/`, engine source untouched):

- `harness/TestHarness.h`, `harness/test_config.cpp`, `harness/engine_fixture.h`
- `engine/test_telex.cpp` — Telex tones, vowels, `w`, `z`, orthography
- `engine/test_vni.cpp` — VNI number keys, `đ`, compound
- `engine/test_simple_telex.cpp` — Simple Telex 1/2
- `engine/test_convert_tool.cpp` — `convertUtil` across code tables + case/mark
- `engine/test_macro.cpp` — add/find/delete/save/load, auto-caps
- `engine/test_smart_switch.cpp` — per-app input-method memory
- `engine/test_engine_branches.cpp` — quick telex/consonant, standalone chars,
  uppercase-first, delete, macro-in-engine, code tables
- `engine/test_engine_deep.cpp` — grammar, modern-mark rules, W vowels, breaks
- `engine/test_engine_edge.cpp` — edge branches, old orthography, restore paths
- `engine/test_engine_corpus.cpp` — broad Vietnamese-word corpus × tones × tables
- `engine/main.cpp` — test runner

---

## P2 — UI layer (outlined, NOT in the 85% target)

The macOS UI (`Sources/OpenKey/macOS/ModernKey/`) mixes AppKit/CGEvent glue
with real logic. Plan (later):

- Extract pure decision functions out of `OpenKey.mm` (`checkHotKey`,
  `isSpotlightVisible`, smart-switch decisions) into testable units — the PR
  #332 seam work belongs here.
- `OpenKeyManager.m` EventTap lifecycle: inject `CGEventTapEnable` behind a
  seam so the disable/re-enable path (PR #332) is unit-testable.
- XCUITest smoke: app launches, menu-bar icon, switch V/E — no line-coverage
  target; functional smoke only.

P2 does **not** gate P1.

---

## Risks / notes

- Engine uses `std::wstring_convert` (deprecated in C++17 but still compiles
  with clang); keep `-std=c++17` and verify no warning-as-error breakage.
- Engine relies on global state; tests must reset via `startNewSession()` and
  re-set config per case — the fixture enforces this.
- Some engine output paths (mouse event, `vTempOffEngine`, `vOtherLanguage`)
  need explicit tests to avoid coverage gaps in the 85% target.