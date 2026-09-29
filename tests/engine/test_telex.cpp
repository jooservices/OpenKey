//
//  test_telex.cpp
//  Telex input mode: tone keys, vowels, double-char vowels, standalone w.
//
#include "TestHarness.h"
#include "engine_fixture.h"

using namespace enginetest;

// "hoas" -> "hoá" (mark s on a). charData[0]=á, charData[1]=KEY_O(raw).
TEST(telex_hoas) {
    resetConfig();
    freshEngine();
    vInputType = 0;
    typeString("hoas");
    auto& h = *hookState();
    EXPECT_EQ(vWillProcess, h.code);
    EXPECT_EQ(2, h.backspaceCount);
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(0xE1, h.charData[0] & CHAR_MASK);  // á
    EXPECT_EQ(KEY_O, h.charData[1] & CHAR_MASK); // o (raw keycode)
}

// "toi" (no mark) -> plain, no mark processing
TEST(telex_plain_word) {
    resetConfig();
    freshEngine();
    vInputType = 0;
    typeString("toi");
    auto& h = *hookState();
    EXPECT_EQ(0, h.backspaceCount);
    EXPECT_EQ(0, h.newCharCount);
}

// "w" -> ư (standalone w)
TEST(telex_standalone_w) {
    resetConfig();
    freshEngine();
    vInputType = 0;
    typeString("w");
    auto& h = *hookState();
    EXPECT_EQ(vWillProcess, h.code);
    EXPECT_EQ(0, h.backspaceCount);
    EXPECT_EQ(1, h.newCharCount);
    EXPECT_EQ(0x01B0, h.charData[0] & CHAR_MASK); // ư
}

// "uow" -> ươ
TEST(telex_uow) {
    resetConfig();
    freshEngine();
    vInputType = 0;
    typeString("uow");
    auto& h = *hookState();
    EXPECT_EQ(vWillProcess, h.code);
    EXPECT_EQ(2, h.backspaceCount);
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(0x01A1, h.charData[0] & CHAR_MASK); // ơ
    EXPECT_EQ(0x01B0, h.charData[1] & CHAR_MASK); // ư
}

// "aa" -> â (the â is produced at the second 'a', before 'n' is typed)
TEST(telex_aan) {
    resetConfig();
    freshEngine();
    vInputType = 0;
    typeString("aa");
    auto& h = *hookState();
    EXPECT_EQ(1, h.newCharCount);
    EXPECT_EQ(0x00E2, h.charData[0] & CHAR_MASK); // â
}

// mark removal with z: "hoas" then z -> "hoa" (a becomes raw keycode again)
TEST(telex_z_removes_mark) {
    resetConfig();
    freshEngine();
    vInputType = 0;
    typeString("hoas");
    typeKey(charToKeycode('z'));
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(KEY_A, h.charData[0] & CHAR_MASK); // a (raw keycode, mark gone)
}

// mark replacement: "hoas" then "f" -> "hoà" (mark moves to last vowel, modern orthography)
TEST(telex_mark_replacement) {
    resetConfig();
    freshEngine();
    vInputType = 0;
    typeString("hoas");
    typeKey(charToKeycode('f'));
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(0x00E0, h.charData[0] & CHAR_MASK); // à
}

// "thees" -> "thế"
TEST(telex_thees) {
    resetConfig();
    freshEngine();
    vInputType = 0;
    typeString("thees");
    auto& h = *hookState();
    EXPECT_EQ(1, h.newCharCount);
    EXPECT_EQ(0x1EBF, h.charData[0] & CHAR_MASK); // ế
}

// modern orthography: "hoas" with vUseModernOrthography=1 -> mark on 'a' (à)
TEST(telex_modern_orthography_on) {
    resetConfig();
    freshEngine();
    vInputType = 0;
    vUseModernOrthography = 1;
    typeString("hoas");
    auto& h = *hookState();
    EXPECT_EQ(0x00E1, h.charData[0] & CHAR_MASK); // á (modern: oà)
}

// old orthography: vUseModernOrthography=0 -> "hóa" (mark on 'o')
TEST(telex_old_orthography) {
    resetConfig();
    freshEngine();
    vInputType = 0;
    vUseModernOrthography = 0;
    typeString("hoas");
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(KEY_A, h.charData[0] & CHAR_MASK); // a (raw keycode)
    EXPECT_EQ(0x00F3, h.charData[1] & CHAR_MASK); // ó
}