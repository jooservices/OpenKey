//
//  test_engine_corpus.cpp
//  Broad corpus of Vietnamese words exercised through the engine with every
//  tone mark, both orthography modes. Asserts invariants on HookState output
//  (bounded counts, sane char codes) — this reaches deep spelling/mark paths.
//
#include "TestHarness.h"
#include "engine_fixture.h"

using namespace enginetest;

static const char* kWords[] = {
    // complex nucleus + coda combinations
    "quyet", "khuyen", "nguyen", "tuyet", "hoai", "loai", "chao", "biet",
    "thuong", "trung", "cuong", "huyen", "quynh", "nguoi", "duoi",
    "truong", "phuong", "thieu", "chieu", "hieu", "kieu", "nhieu",
    "hoan", "hoc", "hong", "hoi", "hon", "hop", "hot", "oai", "oan",
    "quan", "chuyen", "thuyen", "giao", "khoa", "ngoai", "toan",
    "hue", "thu", "tru", "chua", "ngua", "qua", "ghe", "nghe",
    "tiec", "bien", "khien", "nghen", "chen", "xanh", "banh", "manh",
    "sach", "mach", "viet", "thiet", "khiet", "nhiet",
    "thang", "hang", "mang", "tang", "dang", "khang", "phang",
    "thung", "lung", "rung", "nung", "cung", "khung", "chung",
    "mong", "tong", "song", "dung", "trung", "phong", "hong", "cong",
    "khoi", "doi", "moi", "noi", "soi", "toi", "hoi", "thoi",
    "may", "bay", "say", "hay", "tay", "chay", "nay", "vay", "xay",
    "quyen", "thuyt", "khue", "hoa", "thuan", "uong", "oai", "uya",
};
static const int kWordCount = sizeof(kWords) / sizeof(kWords[0]);

static const char kTones[] = {'s', 'f', 'r', 'x', 'j'}; // sắc huyền hỏi ngã nặng

static void corpusWithConfig(int inputType, int codeTable, int modern) {
    for (int w = 0; w < kWordCount; w++) {
        resetConfig();
        freshEngine();
        vInputType = inputType;
        vCodeTable = codeTable;
        vUseModernOrthography = modern;
        typeString(kWords[w]);
        typeKey(charToKeycode(kTones[w % 5]));
        typeKey(KEY_SPACE);
        auto& h = *hookState();
        // invariants: counts stay bounded, code is a known state
        EXPECT_TRUE(h.newCharCount <= MAX_BUFF);
        EXPECT_TRUE(h.backspaceCount <= MAX_BUFF);
        EXPECT_TRUE(h.code == vDoNothing || h.code == vWillProcess ||
                    h.code == vRestore || h.code == vReplaceMaro);
    }
}

TEST(corpus_telex_unicode_modern) {
    corpusWithConfig(vTelex, 0, 1);
}
TEST(corpus_telex_unicode_old) {
    corpusWithConfig(vTelex, 0, 0);
}
TEST(corpus_vni_unicode_modern) {
    corpusWithConfig(vVNI, 0, 1);
}
TEST(corpus_telex_tcvn3) {
    corpusWithConfig(vTelex, 1, 1);
}
TEST(corpus_telex_compound) {
    corpusWithConfig(vTelex, 3, 1);
}
TEST(corpus_telex_cp1258) {
    corpusWithConfig(vTelex, 4, 1);
}
TEST(corpus_simpletelex1_unicode) {
    corpusWithConfig(vSimpleTelex1, 0, 1);
}
TEST(corpus_simpletelex2_unicode) {
    corpusWithConfig(vSimpleTelex2, 0, 1);
}