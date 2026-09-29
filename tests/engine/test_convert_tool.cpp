//
//  test_convert_tool.cpp
//  convertUtil across from/to code tables and case/remove-mark options.
//
#include "TestHarness.h"
#include "engine_fixture.h"
#include "ConvertTool.h"
#include <iomanip>
#include <sstream>

using namespace enginetest;

// helper: compare convertUtil output to expected UTF-8 hex bytes
static bool convertsTo(const std::string& input, const char* expectedHex) {
    std::string out = convertUtil(input);
    std::ostringstream os;
    os << std::hex << std::uppercase << std::setfill('0');
    for (unsigned char c : out)
        os << std::setw(2) << (int)c << ' ';
    return os.str() == std::string(expectedHex);
}

// Unicode -> TCVN3 (1-byte)
TEST(convert_unicode_to_tcvn3) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 1;
    EXPECT_TRUE(convertsTo("hoá", "68 6F C2 B8 "));
}

// remove mark keeps case
TEST(convert_remove_mark) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 0;
    convertToolRemoveMark = true;
    EXPECT_TRUE(convertsTo("hoá", "68 6F 61 "));
}

// all caps
TEST(convert_all_caps) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 0;
    convertToolToAllCaps = true;
    EXPECT_TRUE(convertsTo("hoá", "48 4F C3 81 "));
}

// all non caps
TEST(convert_all_non_caps) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 0;
    convertToolToAllNonCaps = true;
    EXPECT_TRUE(convertsTo("HOÁ", "68 6F C3 A1 "));
}

// caps each word
TEST(convert_caps_each_word) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 0;
    convertToolToCapsEachWord = true;
    EXPECT_TRUE(convertsTo("xin chao", "58 69 6E 20 43 68 61 6F "));
}

// caps first letter
TEST(convert_caps_first_letter) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 0;
    convertToolToCapsFirstLetter = true;
    EXPECT_TRUE(convertsTo("xin chao", "58 69 6E 20 63 68 61 6F "));
}

// Unicode Compound (from=3) -> Unicode
TEST(convert_ucmpd_to_unicode) {
    resetConfig();
    convertToolFromCode = 3;
    convertToolToCode = 0;
    EXPECT_TRUE(convertsTo("ho\u0301a", "68 C3 B3 61 "));
}

// Unicode -> Unicode Compound (to=3)
TEST(convert_unicode_to_ucmpd) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 3;
    EXPECT_TRUE(convertsTo("hoá", "68 6F E2 81 A1 "));
}

// plain text round-trip (no marks) stays as-is
TEST(convert_plain_text) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 1;
    EXPECT_TRUE(convertsTo("abc", "61 62 63 "));
}

// break char handling: ". " caps the next letter with caps-first-letter
TEST(convert_break_caps) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 0;
    convertToolToCapsFirstLetter = true;
    EXPECT_TRUE(convertsTo("xin. chao", "58 69 6E 2E 20 43 68 61 6F "));
}
// Unicode -> VNI-Windows (to=2, double-byte output)
TEST(convert_unicode_to_vni) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 2;
    EXPECT_TRUE(convertsTo("hoá", "68 6F EF A5 A1 "));
}

// Unicode -> CP1258 (to=4, double-byte output)
TEST(convert_unicode_to_cp1258) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 4;
    EXPECT_TRUE(convertsTo("hoá", "68 6F EE B1 A1 "));
}

// remove mark on uppercase source keeps lowercase
TEST(convert_remove_mark_uppercase) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 0;
    convertToolRemoveMark = true;
    EXPECT_TRUE(convertsTo("HOÁ", "68 6F 61 "));
}

// caps-first-letter after '?' break
TEST(convert_break_question) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 0;
    convertToolToCapsFirstLetter = true;
    EXPECT_TRUE(convertsTo("xin? chao", "58 69 6E 3F 20 43 68 61 6F "));
}

// newline resets caps state
TEST(convert_newline) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 0;
    convertToolToCapsFirstLetter = true;
    EXPECT_TRUE(convertsTo("xin\nchao", "58 69 6E 0A 43 68 61 6F "));
}

// to=2 with trailing text exercises the mid-string double-byte branch
TEST(convert_vni_mid_string) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 2;
    EXPECT_TRUE(convertsTo("hoá b", "68 6F 61 C3 B9 20 62 "));
}

// to=4 (CP1258) mid-string
TEST(convert_cp1258_mid_string) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 4;
    EXPECT_TRUE(convertsTo("hoá b", "68 6F 61 C3 AC 20 62 "));
}

// to=3 (Unicode Compound) mid-string
TEST(convert_compound_mid_string) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 3;
    EXPECT_TRUE(convertsTo("hoá b", "68 6F 61 CC 81 20 62 "));
}

// all-caps mid-string (exercises caps selection in the first loop)
TEST(convert_all_caps_mid_string) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 0;
    convertToolToAllCaps = true;
    EXPECT_TRUE(convertsTo("hoá b", "48 4F C3 81 20 42 "));
}

// non-caps mid-string
TEST(convert_non_caps_mid_string) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 0;
    convertToolToAllNonCaps = true;
    EXPECT_TRUE(convertsTo("HOÁ B", "68 6F C3 A1 20 62 "));
}

// remove mark mid-string
TEST(convert_remove_mark_mid_string) {
    resetConfig();
    convertToolFromCode = 0;
    convertToolToCode = 0;
    convertToolRemoveMark = true;
    EXPECT_TRUE(convertsTo("hoá b", "68 6F 61 20 62 "));
}
