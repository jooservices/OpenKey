//
//  test_simple_telex.cpp
//  Simple Telex 1 and 2: mark keys behave like Telex but no z-removal etc.
//
#include "TestHarness.h"
#include "engine_fixture.h"

using namespace enginetest;

// Simple Telex 1: "hoas" -> "hoá"
TEST(st1_hoas) {
    resetConfig();
    freshEngine();
    vInputType = vSimpleTelex1;
    typeString("hoas");
    auto& h = *hookState();
    EXPECT_EQ(0xE1, h.charData[0] & CHAR_MASK);   // á
    EXPECT_EQ(KEY_O, h.charData[1] & CHAR_MASK);
}

// Simple Telex 2: "hoas" -> "hoá"
TEST(st2_hoas) {
    resetConfig();
    freshEngine();
    vInputType = vSimpleTelex2;
    typeString("hoas");
    auto& h = *hookState();
    EXPECT_EQ(0xE1, h.charData[0] & CHAR_MASK);
}

// Simple Telex 1: "w" stays raw (no standalone w in ST1)
TEST(st1_w_raw) {
    resetConfig();
    freshEngine();
    vInputType = vSimpleTelex1;
    typeString("w");
    auto& h = *hookState();
    EXPECT_EQ(0, h.newCharCount);
}

// Simple Telex 1: double a -> â
TEST(st1_aa) {
    resetConfig();
    freshEngine();
    vInputType = vSimpleTelex1;
    typeString("aa");
    auto& h = *hookState();
    EXPECT_EQ(0x00E2, h.charData[0] & CHAR_MASK); // â
}

// Simple Telex 2: double a -> â
TEST(st2_aa) {
    resetConfig();
    freshEngine();
    vInputType = vSimpleTelex2;
    typeString("aa");
    auto& h = *hookState();
    EXPECT_EQ(0x00E2, h.charData[0] & CHAR_MASK);
}