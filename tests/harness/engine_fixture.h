//
//  engine_fixture.h
//  Drives the OpenKey engine state machine for tests.
//
//  The engine is a pure C++ state machine: vKeyHandleEvent() takes a key
//  event and writes its result into a global vKeyHookState (returned by
//  vKeyInit()). Tests type characters (mapped to keycodes via _characterMap)
//  and then inspect the produced HookState.
//
#pragma once

#include <string>
#include <vector>
#include "Engine.h"
#include "Vietnamese.h"

namespace enginetest {

// Map a single ASCII char to its macOS keycode using the engine's own table.
inline Uint16 charToKeycode(char c) {
    auto it = _characterMap.find((Uint32)(unsigned char)c);
    if (it != _characterMap.end())
        return (Uint16)(it->second & CHAR_MASK);
    return 0;
}

inline Uint8 charToCaps(char c) {
    return (c >= 'A' && c <= 'Z') ? 1 : 0;
}

// Reset global config to a known baseline.
inline void resetConfig() {
    vLanguage = 1;
    vInputType = 0;
    vFreeMark = 0;
    vCodeTable = 0;
    vCheckSpelling = 1;
    vUseModernOrthography = 1;
    vQuickTelex = 0;
    vRestoreIfWrongSpelling = 0;
    vFixRecommendBrowser = 1;
    vUseMacro = 0;
    vUseMacroInEnglishMode = 0;
    vAutoCapsMacro = 0;
    vUseSmartSwitchKey = 0;
    vUpperCaseFirstChar = 0;
    vTempOffSpelling = 0;
    vAllowConsonantZFWJ = 0;
    vQuickStartConsonant = 0;
    vQuickEndConsonant = 0;
    vRememberCode = 0;
    vOtherLanguage = 0;
    vTempOffOpenKey = 0;
    convertToolToAllCaps = false;
    convertToolToAllNonCaps = false;
    convertToolToCapsFirstLetter = false;
    convertToolToCapsEachWord = false;
    convertToolRemoveMark = false;
    convertToolFromCode = 0;
    convertToolToCode = 0;
}

// Fresh engine state for a test.
// Note: vKeyInit() does not clear HookState.macroKey/macroData, so leftover
// from a previous test can leak in. Clear those explicitly.
inline void freshEngine() {
    vKeyInit();
    startNewSession();
    vKeyHookState* hook = (vKeyHookState*)vKeyInit();
    hook->macroKey.clear();
    hook->macroData.clear();
    hook->charData[0] = 0;
    hook->backspaceCount = 0;
    hook->newCharCount = 0;
    hook->code = vDoNothing;
    hook->extCode = 0;
}

// Send one key press.
inline void typeKey(Uint16 keycode, Uint8 caps = 0, bool otherControlKey = false) {
    vKeyHandleEvent(vKeyEvent::Keyboard, vKeyEventState::KeyDown, keycode, caps, otherControlKey);
}

// Send one key up (used to exercise KeyUp paths).
inline void releaseKey(Uint16 keycode, Uint8 caps = 0, bool otherControlKey = false) {
    vKeyHandleEvent(vKeyEvent::Keyboard, vKeyEventState::KeyUp, keycode, caps, otherControlKey);
}

// Type a plain ASCII string (each char mapped to a keycode).
// Note: KEY_A == 0, so a keycode of 0 is valid and must not be skipped.
inline void typeString(const std::string& s) {
    for (char c : s) {
        Uint16 kc = charToKeycode(c);
        typeKey(kc, charToCaps(c));
    }
}

// Reconstruct the output the UI layer would send after the last event:
//  - backspaceCount chars removed from the end of `current`
//  - then newCharCount chars from charData[] re-inserted (highest index first)
inline void applyHookState(std::wstring& current, const vKeyHookState& hook) {
    if (hook.backspaceCount > current.size())
        current.clear();
    else
        current.resize(current.size() - hook.backspaceCount);
    for (int i = hook.newCharCount - 1; i >= 0; i--) {
        Uint32 c = hook.charData[i] & CHAR_MASK;
        current.push_back((wchar_t)c);
    }
}

// Stable handle to the engine's global output struct (vKeyInit returns &HookState).
inline vKeyHookState* hookState() {
    static vKeyHookState* hook = (vKeyHookState*)vKeyInit();
    return hook;
}

} // namespace enginetest