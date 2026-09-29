//
//  test_smart_switch.cpp
//  Per-app input method memory (SmartSwitchKey).
//
#include "TestHarness.h"
#include "engine_fixture.h"
#include "SmartSwitchKey.h"

using namespace enginetest;

TEST(ss_unknown_app_returns_minus1) {
    resetConfig();
    initSmartSwitchKey(NULL, 0);
    EXPECT_EQ(-1, getAppInputMethodStatus("com.newapp", 1));
}

TEST(ss_cached_default_returned) {
    resetConfig();
    initSmartSwitchKey(NULL, 0);
    getAppInputMethodStatus("com.newapp", 1); // stores 1
    EXPECT_EQ(1, getAppInputMethodStatus("com.newapp", 0)); // cache hit, ignores param
}

TEST(ss_set_and_get) {
    resetConfig();
    initSmartSwitchKey(NULL, 0);
    setAppInputMethodStatus("com.x", 0);
    EXPECT_EQ(0, getAppInputMethodStatus("com.x", 1));
}

TEST(ss_other_app_not_affected) {
    resetConfig();
    initSmartSwitchKey(NULL, 0);
    setAppInputMethodStatus("com.x", 0);
    EXPECT_EQ(-1, getAppInputMethodStatus("com.y", 1));
}

TEST(ss_save_load_roundtrip) {
    resetConfig();
    initSmartSwitchKey(NULL, 0);
    setAppInputMethodStatus("com.x", 0);
    std::vector<Byte> data;
    getSmartSwitchKeySaveData(data);
    // count(2) + len(1) + "com.x"(5) + value(1) = 9
    EXPECT_EQ(9, (int)data.size());
    initSmartSwitchKey(data.data(), (int)data.size());
    EXPECT_EQ(0, getAppInputMethodStatus("com.x", 1));
}

TEST(ss_init_from_bytes) {
    resetConfig();
    std::vector<Byte> data = {2, 0,
        5, 'c', 'o', 'm', '.', 'a', 1,
        5, 'c', 'o', 'm', '.', 'b', 0};
    initSmartSwitchKey(data.data(), (int)data.size());
    EXPECT_EQ(1, getAppInputMethodStatus("com.a", 0));
    EXPECT_EQ(0, getAppInputMethodStatus("com.b", 1));
}