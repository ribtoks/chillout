#include <float.h>
#include <gtest/gtest.h>
#include <chillout.h>
#include <defines.h>
#include <string>
#include <cstdio>
#include "tests_source.h"

#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

#define WIDEN(quote) WIDEN2(quote)
#define WIDEN2(quote) L##quote

void chilltrace(const char const * stackEntry) {
    if (stackEntry) {
        fprintf(stderr, "chilltrace:  %s", stackEntry);
    }
}

class RecoveryTest : public ::testing::Test
{
protected:
    virtual void SetUp() {
        auto &chillout = Chillout::getInstance();
        chillout.init(L"chillout_test", WIDEN(STRINGIZE(CRASHDUMPSDIR)));
        chillout.setBacktraceCallback(chilltrace);
        chillout.setCrashCallback([](){
                fprintf(stdout, "Crash callback");
            });
    }

    virtual void TearDown() {
        auto &chillout = Chillout::getInstance();
        chillout.deinit();
    }
};

class DISABLED_RecoveryTest : public ::testing::Test
{
};

TEST_F (RecoveryTest, PureVirtualMethodCallTest) {
    ASSERT_EXIT(Derived(), ::testing::ExitedWithCode(CHILLOUT_EXIT_CODE), "::PureCallHandler");
}

TEST_F (RecoveryTest, AccessViolationTest) {
    ASSERT_EXIT(AccessViolation(), ::testing::ExitedWithCode(CHILLOUT_EXIT_CODE), "AccessViolationTest");
}

TEST_F (RecoveryTest, InvalidParameterTest) {
    ASSERT_EXIT(InvalidParameter(), ::testing::ExitedWithCode(CHILLOUT_EXIT_CODE), "::InvalidParameterHandler");
}

TEST_F (RecoveryTest, SigillTest) {
    ASSERT_EXIT(RaiseSigill(), ::testing::ExitedWithCode(CHILLOUT_EXIT_CODE), "::SigillHandler");
}

TEST_F (RecoveryTest, SigsegvTest) {
    ASSERT_EXIT(RaiseSigsegv(), ::testing::ExitedWithCode(CHILLOUT_EXIT_CODE), "::SigsegvHandler");
}

TEST_F (RecoveryTest, SigtermTest) {
    ASSERT_EXIT(RaiseSigterm(), ::testing::ExitedWithCode(CHILLOUT_EXIT_CODE), "::SigtermHandler");
}

// this test is disabled because you can catch c++ exceptions
TEST_F (DISABLED_RecoveryTest, ThrowExceptionTest) {
    ASSERT_EXIT(ThrowException(), ::testing::ExitedWithCode(CHILLOUT_EXIT_CODE), "");
}

TEST_F (RecoveryTest, MemoryTest) {
    ASSERT_EXIT(MemoryOverflow(), ::testing::ExitedWithCode(CHILLOUT_EXIT_CODE), "::NewHandler");
}

TEST_F (DISABLED_RecoveryTest, StackOverflowTest) {
    ASSERT_EXIT(StackOverflow(), ::testing::ExitedWithCode(CHILLOUT_EXIT_CODE), "::adfadf");
}

TEST_F (RecoveryTest, RaiseExceptionTest) {
    ASSERT_EXIT(RaiseSehException(), ::testing::ExitedWithCode(CHILLOUT_EXIT_CODE), "Chillout SehHandler");
}
