#include <float.h>
#include <gtest/gtest.h>
#include <chillout.h>
#include <string>
#include <cstdio>
#include "tests_source.h"

#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

#define WIDEN(quote) WIDEN2(quote)
#define WIDEN2(quote) L##quote

void backtrace(const char const * stackEntry) {
    if (stackEntry) {
        fprintf(stderr, stackEntry);
    }
}

class RecoveryTest : public ::testing::Test
{
  protected:
    virtual void SetUp() {
        auto &chillout = Chillout::getInstance();
        chillout.init(L"chillout_test", WIDEN(STRINGIZE(CRASHDUMPSDIR)));
        chillout.setBacktraceCallback(backtrace);
    }

    virtual void TearDown() {
        auto &chillout = Chillout::getInstance();
        chillout.deinit();
    }
};

TEST_F (RecoveryTest, PureVirtualMethodCallTest) {
    ASSERT_EXIT(Derived(), ::testing::ExitedWithCode(1), "::PureCallHandler");
}

TEST_F (RecoveryTest, AccessViolationTest) {
    ASSERT_EXIT(AccessViolation(), ::testing::ExitedWithCode(1), "");
}

TEST_F (RecoveryTest, InvalidParameterTest) {
    // this test actually does not fail safe because
    // CRT reporting is turned off
    ASSERT_EXIT(InvalidParameter(), ::testing::ExitedWithCode(1), "::InvalidParameterHandler");
}

TEST_F (RecoveryTest, SigillTest) {
    ASSERT_EXIT(RaiseSigill(), ::testing::ExitedWithCode(1), "::SigillHandler");
}

TEST_F (RecoveryTest, SigsegvTest) {
    ASSERT_EXIT(RaiseSigsegv(), ::testing::ExitedWithCode(1), "::SigsegvHandler");
}

TEST_F (RecoveryTest, SigtermTest) {
    ASSERT_EXIT(RaiseSigterm(), ::testing::ExitedWithCode(1), "::SigtermHandler");
}

// // this test is disabled because you can catch c++ exceptions
// TEST_F (RecoveryTest, ThrowExceptionTest) {
//     ASSERT_EXIT(ThrowException(), ::testing::ExitedWithCode(1), "");
// }

TEST_F (RecoveryTest, MemoryTest) {
    ASSERT_EXIT(MemoryOverflow(), ::testing::ExitedWithCode(1), "::NewHandler");
}

// TEST_F (RecoveryTest, StackOverflowTest) {
//     ASSERT_EXIT(StackOverflow(), ::testing::ExitedWithCode(1), "::adfadf");
// }
