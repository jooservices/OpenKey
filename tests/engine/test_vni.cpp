//
//  test_vni.cpp
//  VNI input mode: number-key tone/vowel shortcuts.
//
#include "TestHarness.h"
#include "engine_fixture.h"

using namespace enginetest;

// "hoa1" -> "hoá" (sắc)
TEST(vni_hoa1) {
    resetConfig();
    freshEngine();
    vInputType = vVNI;
    typeString("hoa1");
    auto& h = *hookState();
    EXPECT_EQ(2, h.newCharCount);
    EXPECT_EQ(0xE1, h.charData[0] & CHAR_MASK);   // á
    EXPECT_EQ(KEY_O, h.charData[1] & CHAR_MASK);  // o
}

// "hoa2" -> "hoà" (huyền)
TEST(vni_hoa2) {
    resetConfig();
    freshEngine();
    vInputType = vVNI;
    typeString("hoa2");
    auto& h = *hookState();
    EXPECT_EQ(0xE0, h.charData[0] & CHAR_MASK);   // à
}

// "hoa3" -> "hoả" (hỏi)
TEST(vni_hoa3) {
    resetConfig();
    freshEngine();
    vInputType = vVNI;
    typeString("hoa3");
    auto& h = *hookState();
    EXPECT_EQ(0x1EA3, h.charData[0] & CHAR_MASK); // ả
}

// "hoa4" -> "hoã" (ngã)
TEST(vni_hoa4) {
    resetConfig();
    freshEngine();
    vInputType = vVNI;
    typeString("hoa4");
    auto& h = *hookState();
    EXPECT_EQ(0x00E3, h.charData[0] & CHAR_MASK); // ã
}

// "hoa5" -> "hoạ" (nặng)
TEST(vni_hoa5) {
    resetConfig();
    freshEngine();
    vInputType = vVNI;
    typeString("hoa5");
    auto& h = *hookState();
    EXPECT_EQ(0x1EA1, h.charData[0] & CHAR_MASK); // ạ
}

// "to7" -> "tơ" (ơ)
TEST(vni_to7) {
    resetConfig();
    freshEngine();
    vInputType = vVNI;
    typeString("to7");
    auto& h = *hookState();
    EXPECT_EQ(0x01A1, h.charData[0] & CHAR_MASK); // ơ
}

// "u7" -> "ư"
TEST(vni_u7) {
    resetConfig();
    freshEngine();
    vInputType = vVNI;
    typeString("u7");
    auto& h = *hookState();
    EXPECT_EQ(0x01B0, h.charData[0] & CHAR_MASK); // ư
}

// "o6" -> "ô"
TEST(vni_o6) {
    resetConfig();
    freshEngine();
    vInputType = vVNI;
    typeString("o6");
    auto& h = *hookState();
    EXPECT_EQ(0x00F4, h.charData[0] & CHAR_MASK); // ô
}

// "a8" -> "ă"
TEST(vni_a8) {
    resetConfig();
    freshEngine();
    vInputType = vVNI;
    typeString("a8");
    auto& h = *hookState();
    EXPECT_EQ(0x0103, h.charData[0] & CHAR_MASK); // ă
}

// "tie6ng1" -> "tiếng" (ê via 6, sắc via 1 lands on e)
// charData filled right-to-left: [0]=g, [1]=n, [2]=ế, [3]=i
TEST(vni_tie6ng1) {
    resetConfig();
    freshEngine();
    vInputType = vVNI;
    typeString("tie6ng1");
    auto& h = *hookState();
    EXPECT_EQ(4, h.newCharCount);
    EXPECT_EQ(0x1EBF, h.charData[2] & CHAR_MASK); // ế
}

// "d9" -> đ
TEST(vni_d9) {
    resetConfig();
    freshEngine();
    vInputType = vVNI;
    typeString("d9");
    auto& h = *hookState();
    EXPECT_EQ(1, h.newCharCount);
    EXPECT_EQ(0x0111, h.charData[0] & CHAR_MASK); // đ
}