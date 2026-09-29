//
//  TestHarness.h
//  OpenKey engine unit-test harness (no external deps).
//
#pragma once

#include <cstdio>
#include <string>
#include <vector>

namespace testharness {

struct TestCase {
    const char* name;
    void (*fn)();
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> r;
    return r;
}

inline int& failures() {
    static int f = 0;
    return f;
}

inline int& assertions() {
    static int a = 0;
    return a;
}

inline void report(const char* file, int line, const char* expr, const char* lhs, const char* rhs) {
    failures()++;
    printf("FAIL %s:%d  %s  (lhs=%s rhs=%s)\n", file, line, expr, lhs, rhs);
}

} // namespace testharness

#define TEST(name)                                                     \
    static void test_##name();                                         \
    static struct Register_##name {                                    \
        Register_##name() {                                            \
            testharness::registry().push_back({#name, test_##name});   \
        }                                                              \
    } _reg_##name;                                                     \
    static void test_##name()

#define EXPECT_TRUE(cond)                                                          \
    do {                                                                           \
        testharness::assertions()++;                                               \
        if (!(cond))                                                               \
            testharness::report(__FILE__, __LINE__, #cond, "true", "false");       \
    } while (0)

#define EXPECT_EQ(lhs, rhs)                                                          \
    do {                                                                             \
        testharness::assertions()++;                                                 \
        auto _l = (lhs);                                                             \
        auto _r = (rhs);                                                             \
        if (!(_l == _r))                                                             \
            testharness::report(__FILE__, __LINE__, #lhs " == " #rhs,                \
                                std::to_string((long long)_l).c_str(),               \
                                std::to_string((long long)_r).c_str());              \
    } while (0)

#define RUN_ALL_TESTS()                                                      \
    int main() {                                                             \
        int ran = 0;                                                         \
        int startFails = 0;                                                   \
        int startAssert = 0;                                                  \
        for (auto& t : testharness::registry()) {                            \
            startFails = testharness::failures();                            \
            startAssert = testharness::assertions();                         \
            t.fn();                                                          \
            int df = testharness::failures() - startFails;                   \
            int da = testharness::assertions() - startAssert;                \
            printf("%-60s %s  (%d assert)\n", t.name, df == 0 ? "PASS" : "FAIL", da); \
            ran++;                                                           \
        }                                                                    \
        printf("\nTotal tests: %d, total assertions: %d, total failures: %d\n", \
               ran, testharness::assertions(), testharness::failures());     \
        return testharness::failures() == 0 ? 0 : 1;                         \
    }