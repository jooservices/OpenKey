//
//  test_engine_branches.cpp
//  Engine feature branches: quick telex, quick consonant, spelling restore,
//  standalone chars, uppercase-first, delete, macro-in-engine, code tables,
//  English mode, temp-off.
//
#include "TestHarness.h"
#include "engine_fixture.h"
#include "Macro.h"

using namespace enginetest;

// ---- Quick Telex ----
TEST(engine_quick_telex_cc) {
    resetConfig();
    freshEngine();
    vQuickTelex = 1;
    typeKey(charToKeycode('c'));
    typeKey(charToKeycode('c'));
    auto& h = *hookState();
    EXPECT_EQ(1, h.backspaceCount);
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(KEY_H, h.charData[0] & CHAR_MASK);  // h
    EXPECT_EQ(KEY_C, h.charData[1] & CHAR_MASK);  // c
}

TEST(engine_quick_telex_gg) {
    resetConfig();
    freshEngine();
    vQuickTelex = 1;
    typeKey(charToKeycode('g'));
    typeKey(charToKeycode('g'));
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(KEY_I, h.charData[0] & CHAR_MASK); // i
    EXPECT_EQ(KEY_G, h.charData[1] & CHAR_MASK); // g
}

// ---- Quick start consonant: f -> ph ----
TEST(engine_quick_start_consonant) {
    resetConfig();
    freshEngine();
    vQuickStartConsonant = 1;
    typeKey(charToKeycode('f'));
    typeKey(charToKeycode('a'));
    typeKey(charToKeycode('n'));
    typeKey(KEY_SPACE);
    auto& h = *hookState();
    EXPECT_EQ(4, h.newCharCount);
    EXPECT_EQ(KEY_N, h.charData[0] & CHAR_MASK);
}

// ---- Quick end consonant: g -> ng ----
TEST(engine_quick_end_consonant) {
    resetConfig();
    freshEngine();
    vQuickEndConsonant = 1;
    typeKey(charToKeycode('b'));
    typeKey(charToKeycode('a'));
    typeKey(charToKeycode('g'));
    typeKey(KEY_SPACE);
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(KEY_G, h.charData[0] & CHAR_MASK); // g
    EXPECT_EQ(KEY_N, h.charData[1] & CHAR_MASK); // n
}

// ---- Standalone chars: tr[ -> ơ ----
TEST(engine_standalone_left_bracket) {
    resetConfig();
    freshEngine();
    typeKey(charToKeycode('t'));
    typeKey(charToKeycode('r'));
    typeKey(KEY_LEFT_BRACKET);
    auto& h = *hookState();
    EXPECT_EQ(0x01A1, h.charData[0] & CHAR_MASK); // ơ
}

TEST(engine_standalone_right_bracket) {
    resetConfig();
    freshEngine();
    typeKey(charToKeycode('t'));
    typeKey(charToKeycode('r'));
    typeKey(KEY_RIGHT_BRACKET);
    auto& h = *hookState();
    EXPECT_EQ(0x01B0, h.charData[0] & CHAR_MASK); // ư
}

// ---- Uppercase first char after dot ----
TEST(engine_uppercase_first_char) {
    resetConfig();
    freshEngine();
    vUpperCaseFirstChar = 1;
    typeKey(charToKeycode('a'));
    typeKey(KEY_DOT);
    typeKey(KEY_SPACE);
    typeKey(charToKeycode('b'), 0);
    auto& h = *hookState();
    EXPECT_EQ(1, h.newCharCount);
    EXPECT_EQ(KEY_B | CAPS_MASK, h.charData[0] & (CHAR_MASK | CAPS_MASK));
}

// ---- Delete key after word ----
TEST(engine_delete) {
    resetConfig();
    freshEngine();
    typeKey(charToKeycode('h'));
    typeKey(charToKeycode('o'));
    typeKey(KEY_DELETE);
    auto& h = *hookState();
    EXPECT_EQ(0, h.backspaceCount);
}

// ---- Macro triggered through engine typing ----
TEST(engine_macro_in_engine) {
    resetConfig();
    freshEngine();
    initMacroMap(NULL, 0);
    vUseMacro = 1;
    addMacro("btw", "by the way");
    typeKey(charToKeycode('b'));
    typeKey(charToKeycode('t'));
    typeKey(charToKeycode('w'));
    typeKey(KEY_SPACE);
    auto& h = *hookState();
    EXPECT_EQ(vReplaceMaro, h.code);
    EXPECT_EQ(3, h.backspaceCount);
}

// ---- D key -> đ ----
TEST(engine_double_d) {
    resetConfig();
    freshEngine();
    typeKey(charToKeycode('d'));
    typeKey(charToKeycode('d'));
    auto& h = *hookState();
    EXPECT_EQ(1, h.newCharCount);
    EXPECT_EQ(0x0111, h.charData[0] & CHAR_MASK); // đ
}

// ---- Code tables produce distinct bytes ----
TEST(engine_table_vni_windows) {
    resetConfig();
    freshEngine();
    vCodeTable = 2;
    typeString("hoas");
    auto& h = *hookState();
    EXPECT_EQ(0xF961, h.charData[0] & CHAR_MASK); // á (VNI-Windows)
}

TEST(engine_table_tcvn3) {
    resetConfig();
    freshEngine();
    vCodeTable = 1;
    typeString("hoas");
    auto& h = *hookState();
    EXPECT_EQ(0xB8, h.charData[0] & CHAR_MASK); // á (TCVN3)
}

TEST(engine_table_compound) {
    resetConfig();
    freshEngine();
    vCodeTable = 3;
    typeString("hoas");
    auto& h = *hookState();
    EXPECT_EQ(0x2061, h.charData[0] & CHAR_MASK); // á (Unicode Compound)
}

TEST(engine_table_cp1258) {
    resetConfig();
    freshEngine();
    vCodeTable = 4;
    typeString("hoas");
    auto& h = *hookState();
    EXPECT_EQ(0xEC61, h.charData[0] & CHAR_MASK); // á (CP1258)
}

// ---- English mode / temp off ----
TEST(engine_english_mode) {
    resetConfig();
    freshEngine();
    vEnglishMode(vKeyEventState::KeyDown, charToKeycode('h'), 0, false);
    auto& h = *hookState();
    EXPECT_EQ(vDoNothing, h.code);
}

TEST(engine_temp_off_engine) {
    resetConfig();
    freshEngine();
    vTempOffEngine(true);
    typeKey(charToKeycode('h'));
    auto& h = *hookState();
    EXPECT_EQ(vDoNothing, h.code);
}

TEST(engine_temp_off_spelling_toggle) {
    resetConfig();
    freshEngine();
    vSetCheckSpelling();
    vTempOffSpellChecking();
    EXPECT_EQ(0, vCheckSpelling);
    vTempOffSpellChecking();
    EXPECT_EQ(1, vCheckSpelling);
}

// ---- Word break: number key at word start stays raw ----
TEST(engine_number_at_start) {
    resetConfig();
    freshEngine();
    typeKey(charToKeycode('5'));
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}

// ---- Number with shift -> raw symbol ----
TEST(engine_number_with_shift) {
    resetConfig();
    freshEngine();
    typeKey(charToKeycode('5'), 1);
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}

// ---- otherControlKey bypasses engine ----
TEST(engine_control_key_bypass) {
    resetConfig();
    freshEngine();
    typeKey(charToKeycode('a'), 0, true);
    auto& h = *hookState();
    EXPECT_EQ(vDoNothing, h.code);
}

// ---- vOtherLanguage + TIS check path needs engine input source; skip logic ----
TEST(engine_fix_recommend_browser_flag) {
    resetConfig();
    freshEngine();
    vFixRecommendBrowser = 1;
    // does not crash and plain typing works
    typeKey(charToKeycode('t'));
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}