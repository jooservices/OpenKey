//
//  test_macro.cpp
//  Macro map: add/find/delete/has, save/load round-trip, table-code reload.
//
#include "TestHarness.h"
#include "engine_fixture.h"
#include "Macro.h"

using namespace enginetest;

TEST(macro_add_and_has) {
    resetConfig();
    initMacroMap(NULL, 0);
    addMacro("btw", "by the way");
    EXPECT_EQ(1, hasMacro("btw"));
    EXPECT_EQ(0, hasMacro("nope"));
}

TEST(macro_find_content) {
    resetConfig();
    initMacroMap(NULL, 0);
    addMacro("btw", "by the way");
    std::vector<Uint32> key = {KEY_B, KEY_T, KEY_W};
    std::vector<Uint32> content;
    EXPECT_EQ(1, findMacro(key, content));
    EXPECT_EQ(10, (int)content.size());
}

TEST(macro_delete) {
    resetConfig();
    initMacroMap(NULL, 0);
    addMacro("btw", "by the way");
    EXPECT_EQ(1, deleteMacro("btw"));
    EXPECT_EQ(0, hasMacro("btw"));
    EXPECT_EQ(0, deleteMacro("btw"));
}

TEST(macro_save_load_roundtrip) {
    resetConfig();
    initMacroMap(NULL, 0);
    addMacro("ms", "millisecond");
    std::vector<Byte> data;
    getMacroSaveData(data);
    EXPECT_EQ(18, (int)data.size());
    initMacroMap(data.data(), (int)data.size());
    EXPECT_EQ(1, hasMacro("ms"));
}

TEST(macro_file_roundtrip) {
    resetConfig();
    initMacroMap(NULL, 0);
    addMacro("ms", "millisecond");
    const char* path = "/tmp/okey-test-macros.txt";
    saveToFile(path);
    initMacroMap(NULL, 0);
    EXPECT_EQ(0, hasMacro("ms"));
    readFromFile(path, false);
    EXPECT_EQ(1, hasMacro("ms"));
}

TEST(macro_table_code_change) {
    resetConfig();
    initMacroMap(NULL, 0);
    addMacro("d", "đá");
    onTableCodeChange();
    EXPECT_EQ(1, hasMacro("d"));
}

TEST(macro_get_all) {
    resetConfig();
    initMacroMap(NULL, 0); // isolate from previous tests
    addMacro("a1", "one");
    addMacro("a2", "two");
    std::vector<std::vector<Uint32>> keys;
    std::vector<std::string> texts, contents;
    getAllMacro(keys, texts, contents);
    EXPECT_EQ(2, (int)texts.size());
}

TEST(macro_auto_caps) {
    resetConfig();
    initMacroMap(NULL, 0);
    vAutoCapsMacro = 1;
    addMacro("btw", "by the way");
    std::vector<Uint32> key = {KEY_B | CAPS_MASK, KEY_T, KEY_W};
    std::vector<Uint32> content;
    EXPECT_EQ(1, findMacro(key, content));
}
// auto-caps with unicode content (modifyCaseUnicode unicode branch)
TEST(macro_auto_caps_unicode_content) {
    resetConfig();
    initMacroMap(NULL, 0);
    vAutoCapsMacro = 1;
    addMacro("h", "Hoa");
    std::vector<Uint32> key = {KEY_H | CAPS_MASK};
    std::vector<Uint32> content;
    EXPECT_EQ(1, findMacro(key, content));
    EXPECT_EQ(3, (int)content.size());
}

// auto-caps with accented content
TEST(macro_auto_caps_accent) {
    resetConfig();
    initMacroMap(NULL, 0);
    vAutoCapsMacro = 1;
    addMacro("a", "à");
    std::vector<Uint32> key = {KEY_A | CAPS_MASK};
    std::vector<Uint32> content;
    EXPECT_EQ(1, findMacro(key, content));
}

// readFromFile handles colon inside name/content
TEST(macro_read_from_file_colon) {
    resetConfig();
    initMacroMap(NULL, 0);
    const char* path = "/tmp/okey-test-colon.txt";
    FILE* f = fopen(path, "w");
    fputs(";Compatible\n", f);
    fputs(":abc\n", f);
    fputs("ab:cd:ef\n", f);
    fclose(f);
    readFromFile(path, false);
    EXPECT_EQ(1, hasMacro("ab"));
    EXPECT_EQ(0, hasMacro(":abc"));
}

// delete non-existent returns false
TEST(macro_delete_missing) {
    resetConfig();
    initMacroMap(NULL, 0);
    EXPECT_EQ(0, deleteMacro("nope"));
}
