//
//  test_engine_deep.cpp
//  Deep engine branches: grammar, modern-mark rules, W vowel handling,
//  long words, word breaks, delete paths.
//
#include "TestHarness.h"
#include "engine_fixture.h"
#include "Macro.h"

using namespace enginetest;

// ---- Grammar: "thuong" + w -> "thương" ----
TEST(engine_grammar_thuongw) {
    resetConfig();
    freshEngine();
    typeString("thuongw");
    auto& h = *hookState();
    EXPECT_EQ(4, h.newCharCount);
    EXPECT_EQ(0x01B0, h.charData[3] & CHAR_MASK); // ư
    EXPECT_EQ(0x01A1, h.charData[2] & CHAR_MASK); // ơ
}

// ---- Modern mark rule: "quys" -> "quỳ" (mark on y) ----
TEST(engine_modern_quys) {
    resetConfig();
    freshEngine();
    vUseModernOrthography = 1;
    typeString("quys");
    auto& h = *hookState();
    EXPECT_EQ(1, h.newCharCount);
    EXPECT_EQ(0x00FD, h.charData[0] & CHAR_MASK); // ỳ
}

// ---- Modern mark rule: "quyes" -> "quý" (uý) ----
TEST(engine_modern_quyes) {
    resetConfig();
    freshEngine();
    vUseModernOrthography = 1;
    typeString("quyes");
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(0x00FD, h.charData[1] & CHAR_MASK); // y
}

// ---- Modern mark rule: "tias" -> "tía" (mark on i) ----
TEST(engine_modern_tias) {
    resetConfig();
    freshEngine();
    vUseModernOrthography = 1;
    typeString("tias");
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(0x00ED, h.charData[1] & CHAR_MASK); // í
}

// ---- W vowel: "huow" -> "hươ" ----
TEST(engine_huow) {
    resetConfig();
    freshEngine();
    typeString("huow");
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(0x01A1, h.charData[0] & CHAR_MASK); // ơ
    EXPECT_EQ(0x01B0, h.charData[1] & CHAR_MASK); // ư
}

// ---- W vowel restore: "uoww" -> restore to raw ----
TEST(engine_uoww_restore) {
    resetConfig();
    freshEngine();
    typeString("uoww");
    auto& h = *hookState();
    EXPECT_EQ(vRestore, h.code);
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(KEY_O, h.charData[0] & CHAR_MASK); // o (raw, restored)
    EXPECT_EQ(KEY_U, h.charData[1] & CHAR_MASK); // u
}

// ---- Long word (> 32 chars) does not crash ----
TEST(engine_long_word) {
    resetConfig();
    freshEngine();
    typeString("abcdefghijklmnopqrstuvwxyzabcdefghij");
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}

// ---- Word break: comma and semicolon ----
TEST(engine_word_break_comma) {
    resetConfig();
    freshEngine();
    typeKey(charToKeycode('h'));
    typeKey(KEY_COMMA);
    typeKey(charToKeycode('o'));
    typeKey(KEY_SEMICOLON);
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}

// ---- Enter / Return break ----
TEST(engine_enter_break) {
    resetConfig();
    freshEngine();
    typeKey(charToKeycode('h'));
    typeKey(KEY_RETURN);
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}

// ---- Tab break ----
TEST(engine_tab_break) {
    resetConfig();
    freshEngine();
    typeKey(charToKeycode('h'));
    typeKey(KEY_TAB);
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}

// ---- Delete in the middle of a word ----
TEST(engine_delete_middle) {
    resetConfig();
    freshEngine();
    typeString("hoa");
    typeKey(KEY_DELETE);
    typeKey(KEY_SPACE);
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}

// ---- Space then delete then space ----
TEST(engine_space_delete_space) {
    resetConfig();
    freshEngine();
    typeKey(charToKeycode('h'));
    typeKey(KEY_SPACE);
    typeKey(KEY_DELETE);
    typeKey(KEY_SPACE);
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}

// ---- 'w' standalone after 'q' -> qu (no ư) ----
TEST(engine_quw) {
    resetConfig();
    freshEngine();
    typeString("quw");
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}

// ---- 'd' after consonant allows đ correction ----
TEST(engine_d_after_consonant) {
    resetConfig();
    freshEngine();
    typeString("bd");
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}