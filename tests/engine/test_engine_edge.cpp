//
//  test_engine_edge.cpp
//  Edge branches: old-orthography 3-vowel marks, insertD/AOE/W restore,
//  standalone w contexts, macro + English-mode interactions, mouse.
//
#include "TestHarness.h"
#include "engine_fixture.h"
#include "Macro.h"

using namespace enginetest;

// ---- Old orthography 3-vowel: "nghieus" ----
TEST(edge_old_3vowel) {
    resetConfig();
    freshEngine();
    vUseModernOrthography = 0;
    typeString("nghieus");
    auto& h = *hookState();
    EXPECT_EQ(3, h.newCharCount);
    EXPECT_EQ(0x00E9, h.charData[1] & CHAR_MASK); // é
}

// ---- insertD restore: "dad" ----
TEST(edge_dad_restore) {
    resetConfig();
    freshEngine();
    typeString("dad");
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(0x0111, h.charData[1] & CHAR_MASK); // đ
}

// ---- insertAOE restore: "aaa" removes ^ ----
TEST(edge_aaa_restore) {
    resetConfig();
    freshEngine();
    typeString("aaa");
    auto& h = *hookState();
    EXPECT_EQ(vRestore, h.code);
    EXPECT_EQ(1, h.newCharCount);
    EXPECT_EQ(KEY_A, h.charData[0] & CHAR_MASK); // a raw
}

// ---- insertW restore: "uww" ----
TEST(edge_uww_restore) {
    resetConfig();
    freshEngine();
    typeString("uww");
    auto& h = *hookState();
    EXPECT_EQ(vRestore, h.code);
    EXPECT_EQ(1, h.newCharCount);
    EXPECT_EQ(KEY_U, h.charData[0] & CHAR_MASK); // u raw
}

// ---- "truw" -> "trư" (standalone w) ----
TEST(edge_truw) {
    resetConfig();
    freshEngine();
    typeString("truw");
    auto& h = *hookState();
    EXPECT_EQ(1, h.newCharCount);
    EXPECT_EQ(0x01B0, h.charData[0] & CHAR_MASK); // ư
}

// ---- "trw" -> "trư" (w after tr) ----
TEST(edge_trw) {
    resetConfig();
    freshEngine();
    typeString("trw");
    auto& h = *hookState();
    EXPECT_EQ(1, h.newCharCount);
    EXPECT_EQ(0x01B0, h.charData[0] & CHAR_MASK); // ư
}

// ---- macro delete cancels match ----
TEST(edge_macro_delete) {
    resetConfig();
    freshEngine();
    initMacroMap(NULL, 0);
    vUseMacro = 1;
    addMacro("btw", "by the way");
    typeKey(charToKeycode('b'));
    typeKey(charToKeycode('t'));
    typeKey(charToKeycode('w'));
    typeKey(KEY_DELETE);
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}

// ---- English mode accumulates then space finds macro ----
TEST(edge_english_macro_space) {
    resetConfig();
    freshEngine();
    initMacroMap(NULL, 0);
    vUseMacro = 1;
    addMacro("btw", "by the way");
    vEnglishMode(vKeyEventState::KeyDown, charToKeycode('b'), 0, false);
    vEnglishMode(vKeyEventState::KeyDown, charToKeycode('t'), 0, false);
    vEnglishMode(vKeyEventState::KeyDown, charToKeycode('w'), 0, false);
    vEnglishMode(vKeyEventState::KeyDown, KEY_SPACE, 0, false);
    auto& h = *hookState();
    EXPECT_EQ(vReplaceMaro, h.code);
}

// ---- English mode delete pops macro key ----
TEST(edge_english_delete) {
    resetConfig();
    freshEngine();
    vUseMacro = 1;
    vEnglishMode(vKeyEventState::KeyDown, charToKeycode('b'), 0, false);
    vEnglishMode(vKeyEventState::KeyDown, KEY_DELETE, 0, false);
    auto& h = *hookState();
    EXPECT_EQ(vDoNothing, h.code);
}

// ---- Mouse down resets session ----
TEST(edge_mouse_down) {
    resetConfig();
    freshEngine();
    vKeyHandleEvent(vKeyEvent::Mouse, vKeyEventState::MouseDown, 0, 0, false);
    auto& h = *hookState();
    EXPECT_EQ(vDoNothing, h.code);
}

// ---- "thuongw" (grammar N/U/O path already covered) ----
TEST(edge_bracket_simpletelex) {
    resetConfig();
    freshEngine();
    vInputType = vSimpleTelex1;
    typeKey(charToKeycode('a'));
    typeKey(KEY_LEFT_BRACKET);
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}
// ---- checkSpelling 'ch'/'t' mark limits ----
TEST(edge_spelling_ch_mark) {
    resetConfig();
    freshEngine();
    typeString("bach");
    typeKey(charToKeycode('s'));
    auto& h = *hookState();
    EXPECT_EQ(3, h.newCharCount);
    EXPECT_EQ(0x00E1, h.charData[2] & CHAR_MASK); // á
}

TEST(edge_spelling_t_mark) {
    resetConfig();
    freshEngine();
    typeString("mat");
    typeKey(charToKeycode('s'));
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(0x00E1, h.charData[1] & CHAR_MASK); // á
}

// ---- long word then space (saveWord overflow path) ----
TEST(edge_long_word_space) {
    resetConfig();
    freshEngine();
    typeString("abcdefghijklmnopqrstuvwxyzabcdefghijklmnop");
    typeKey(KEY_SPACE);
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}

// ---- restore last typing state via delete ----
TEST(edge_delete_restore_state) {
    resetConfig();
    freshEngine();
    typeString("hoas");
    typeKey(KEY_DELETE);
    typeKey(KEY_DELETE);
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}

// ---- modern mark rule 4 'io'/'iu' ----
TEST(edge_modern_tios) {
    resetConfig();
    freshEngine();
    typeString("tios");
    auto& h = *hookState();
    EXPECT_EQ(0x00ED, h.charData[1] & CHAR_MASK); // í
}

TEST(edge_modern_tius) {
    resetConfig();
    freshEngine();
    typeString("tius");
    auto& h = *hookState();
    EXPECT_EQ(0x00ED, h.charData[1] & CHAR_MASK); // í
}

// ---- old orthography 3-vowel ----
TEST(edge_old_ngheus) {
    resetConfig();
    freshEngine();
    vUseModernOrthography = 0;
    typeString("ngheus");
    auto& h = *hookState();
    EXPECT_EQ(0x00E9, h.charData[1] & CHAR_MASK); // é
}

// ---- insertAOE 'o' restore: "ooo" ----
TEST(edge_ooo_restore) {
    resetConfig();
    freshEngine();
    typeString("ooo");
    auto& h = *hookState();
    EXPECT_EQ(vRestore, h.code);
    EXPECT_EQ(KEY_O, h.charData[0] & CHAR_MASK); // o raw
}

// ---- 'heow' -> ươ?? ----
TEST(edge_heow) {
    resetConfig();
    freshEngine();
    typeString("heow");
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
}

// ---- 'hauw' -> ấu path ----
TEST(edge_hauw) {
    resetConfig();
    freshEngine();
    typeString("hauw");
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
}

// ---- uppercase first char after enter ----
TEST(edge_uppercase_after_enter) {
    resetConfig();
    freshEngine();
    vUpperCaseFirstChar = 1;
    typeKey(charToKeycode('a'));
    typeKey(KEY_ENTER);
    typeKey(charToKeycode('b'));
    auto& h = *hookState();
    EXPECT_EQ(1, h.newCharCount);
    EXPECT_EQ(KEY_B | CAPS_MASK, h.charData[0] & (CHAR_MASK | CAPS_MASK));
}

// ---- grammar N-key path: "thuon"+s ----
TEST(edge_grammar_thuons) {
    resetConfig();
    freshEngine();
    typeString("thuons");
    auto& h = *hookState();
    EXPECT_EQ(3, h.newCharCount);
    EXPECT_EQ(0x00F3, h.charData[1] & CHAR_MASK); // ó
}

// ---- grammar with ng: "thuongs" ----
TEST(edge_grammar_thuongs) {
    resetConfig();
    freshEngine();
    typeString("thuongs");
    auto& h = *hookState();
    EXPECT_EQ(4, h.newCharCount);
    EXPECT_EQ(0x00F3, h.charData[2] & CHAR_MASK); // ó
}

// ---- canHasEndConsonant: "quots" ----
TEST(edge_quots) {
    resetConfig();
    freshEngine();
    typeString("quots");
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(0x00F3, h.charData[1] & CHAR_MASK); // ó
}

// ---- English mode mouse down ----
TEST(edge_english_mouse) {
    resetConfig();
    freshEngine();
    vEnglishMode(vKeyEventState::MouseDown, 0, 0, false);
    auto& h = *hookState();
    EXPECT_EQ(vDoNothing, h.code);
}

// ---- English mode control key ----
TEST(edge_english_control) {
    resetConfig();
    freshEngine();
    vEnglishMode(vKeyEventState::KeyDown, charToKeycode('a'), 0, true);
    auto& h = *hookState();
    EXPECT_EQ(vDoNothing, h.code);
}

// ---- English mode word break comma ----
TEST(edge_english_comma) {
    resetConfig();
    freshEngine();
    vEnglishMode(vKeyEventState::KeyDown, KEY_COMMA, 0, false);
    auto& h = *hookState();
    EXPECT_EQ(vDoNothing, h.code);
}

// ---- English mode delete with empty macro key ----
TEST(edge_english_delete_empty) {
    resetConfig();
    freshEngine();
    vEnglishMode(vKeyEventState::KeyDown, KEY_DELETE, 0, false);
    auto& h = *hookState();
    EXPECT_EQ(vDoNothing, h.code);
}

// ---- macro word processed then space (vWillProcess macro restore path) ----
TEST(edge_macro_word_space) {
    resetConfig();
    freshEngine();
    initMacroMap(NULL, 0);
    vUseMacro = 1;
    typeString("hoas");
    typeKey(KEY_SPACE);
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(0x00E1, h.charData[0] & CHAR_MASK); // á
}

// ---- delete after special char ----
TEST(edge_delete_special_char) {
    resetConfig();
    freshEngine();
    typeKey(charToKeycode('h'));
    typeKey(KEY_DOT);
    typeKey(KEY_DELETE);
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}

// ---- modern mark rules: multi-vowel combos ----
TEST(edge_modern_uyus) {
    resetConfig(); freshEngine();
    typeString("uyus");
    auto& h = *hookState();
    EXPECT_EQ(3, h.newCharCount);
    EXPECT_EQ(0x00FD, h.charData[1] & CHAR_MASK); // ỳ
}
TEST(edge_modern_uous) {
    resetConfig(); freshEngine();
    typeString("uous");
    auto& h = *hookState();
    EXPECT_EQ(3, h.newCharCount);
    EXPECT_EQ(0x00F3, h.charData[1] & CHAR_MASK); // ó
}
TEST(edge_modern_uyans) {
    resetConfig(); freshEngine();
    typeString("uyans");
    auto& h = *hookState();
    EXPECT_EQ(4, h.newCharCount);
    EXPECT_EQ(0x00FD, h.charData[2] & CHAR_MASK); // ỳ
}
TEST(edge_modern_oeos) {
    resetConfig(); freshEngine();
    typeString("oeos");
    auto& h = *hookState();
    EXPECT_EQ(3, h.newCharCount);
    EXPECT_EQ(0x00E9, h.charData[1] & CHAR_MASK); // é
}
TEST(edge_modern_oais) {
    resetConfig(); freshEngine();
    typeString("oais");
    auto& h = *hookState();
    EXPECT_EQ(3, h.newCharCount);
    EXPECT_EQ(0x00E1, h.charData[1] & CHAR_MASK); // á
}
TEST(edge_modern_oays) {
    resetConfig(); freshEngine();
    typeString("oays");
    auto& h = *hookState();
    EXPECT_EQ(3, h.newCharCount);
    EXPECT_EQ(0x00E1, h.charData[1] & CHAR_MASK); // á
}
TEST(edge_modern_uoas) {
    resetConfig(); freshEngine();
    typeString("uoas");
    auto& h = *hookState();
    EXPECT_EQ(3, h.newCharCount);
    EXPECT_EQ(0x00F3, h.charData[1] & CHAR_MASK); // ó
}
TEST(edge_modern_iues) {
    resetConfig(); freshEngine();
    typeString("iues");
    auto& h = *hookState();
    EXPECT_EQ(3, h.newCharCount);
    EXPECT_EQ(0x00FA, h.charData[1] & CHAR_MASK); // ú
}

// ---- canHasEndConsonant: "quocs" ----
TEST(edge_quocs) {
    resetConfig(); freshEngine();
    typeString("quocs");
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(0x00F3, h.charData[1] & CHAR_MASK); // ó
}
TEST(edge_huots) {
    resetConfig(); freshEngine();
    typeString("huots");
    auto& h = *hookState();
    EXPECT_EQ(3, h.newCharCount);
    EXPECT_EQ(0x00F3, h.charData[1] & CHAR_MASK); // ó
}
// ---- insertW 'uo'+n: "thuonw" ----
TEST(edge_thuonw) {
    resetConfig(); freshEngine();
    typeString("thuonw");
    auto& h = *hookState();
    EXPECT_EQ(3, h.newCharCount);
    EXPECT_EQ(0x01B0, h.charData[2] & CHAR_MASK); // ư
    EXPECT_EQ(0x01A1, h.charData[1] & CHAR_MASK); // ơ
}

// ---- macro triggered by break char (comma) ----
TEST(edge_macro_comma_break) {
    resetConfig(); freshEngine();
    initMacroMap(NULL, 0);
    vUseMacro = 1;
    addMacro("btw", "by the way");
    typeKey(charToKeycode('b'));
    typeKey(charToKeycode('t'));
    typeKey(charToKeycode('w'));
    typeKey(KEY_COMMA);
    auto& h = *hookState();
    EXPECT_EQ(vReplaceMaro, h.code);
    EXPECT_EQ(3, h.backspaceCount);
}

// ---- "gins" -> "gín" (gi consonant handling) ----
TEST(edge_gins) {
    resetConfig(); freshEngine();
    typeString("gins");
    auto& h = *hookState();
    EXPECT_EQ(3, h.newCharCount);
    EXPECT_EQ(0x00ED, h.charData[1] & CHAR_MASK); // í
}
// ---- "quets" -> "quét" ----
TEST(edge_quets) {
    resetConfig(); freshEngine();
    typeString("quets");
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(0x00E9, h.charData[1] & CHAR_MASK); // é
}
// ---- old orthography "heos" ----
TEST(edge_old_heos) {
    resetConfig(); freshEngine();
    vUseModernOrthography = 0;
    typeString("heos");
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(0x00E9, h.charData[1] & CHAR_MASK); // é
}
// ---- old orthography "thoongs" ----
TEST(edge_old_thoongs) {
    resetConfig(); freshEngine();
    vUseModernOrthography = 0;
    typeString("thoongs");
    auto& h = *hookState();
    EXPECT_EQ(3, h.newCharCount);
    EXPECT_EQ(0x1ED1, h.charData[2] & CHAR_MASK); // ố
}
// ---- "did" -> "đi" ----
TEST(edge_did) {
    resetConfig(); freshEngine();
    typeString("did");
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(0x0111, h.charData[1] & CHAR_MASK); // đ
}

// ---- insertW vowelCount>1: "uoiw" ----
TEST(edge_uoiw) {
    resetConfig(); freshEngine();
    typeString("uoiw");
    auto& h = *hookState();
    EXPECT_EQ(3, h.newCharCount);
    EXPECT_EQ(0x01B0, h.charData[2] & CHAR_MASK); // ư
    EXPECT_EQ(0x01A1, h.charData[1] & CHAR_MASK); // ơ
}
// ---- "uoiww" restore ----
TEST(edge_uoiww_restore) {
    resetConfig(); freshEngine();
    typeString("uoiww");
    auto& h = *hookState();
    EXPECT_EQ(vRestore, h.code);
    EXPECT_EQ(3, h.newCharCount);
}
// ---- "iaow" ----
TEST(edge_iaow) {
    resetConfig(); freshEngine();
    typeString("iaow");
    auto& h = *hookState();
    EXPECT_EQ(3, h.newCharCount);
}
