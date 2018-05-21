#include <float.h>
#include <gtest/gtest.h>
#include <chillout.h>
#include <defines.h>
#include <string>
#include <cstdio>
#include "tests_source.h"
#include "gtest_extensions.h"
#include <signal.h>

#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

#define WIDEN(quote) WIDEN2(quote)
#define WIDEN2(quote) L##quote

void chilltrace(const char * const stackEntry) {
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
                fprintf(stderr, "Crash callback");
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
    ASSERT_EXIT(Derived(), ::extensions::ExitedOrKilled(CHILLOUT_EXIT_CODE, SIGSEGV), "(::PureCallHandler|PosixCrashHandler::handleCrash)");
}

TEST_F (RecoveryTest, AccessViolationTest) {
    ASSERT_EXIT(AccessViolation(), ::extensions::ExitedOrKilled(CHILLOUT_EXIT_CODE, SIGSEGV), "AccessViolationTest");
}

TEST_F (RecoveryTest, InvalidParameterTest) {
    ASSERT_EXIT(InvalidParameter(), ::extensions::ExitedOrKilled(CHILLOUT_EXIT_CODE, SIGSEGV), "(::InvalidParameterHandler|PosixCrashHandler::handleCrash)");
}

TEST_F (RecoveryTest, SigillTest) {
    ASSERT_EXIT(RaiseSigill(), ::extensions::ExitedOrKilled(CHILLOUT_EXIT_CODE, SIGILL), "(::SigillHandler|posixSignalHandler)");
}

TEST_F (RecoveryTest, SigsegvTest) {
    ASSERT_EXIT(RaiseSigsegv(), ::extensions::ExitedOrKilled(CHILLOUT_EXIT_CODE, SIGSEGV), "(::SigsegvHandler|posixSignalHandler)");
}

TEST_F (RecoveryTest, SigtermTest) {
    ASSERT_EXIT(RaiseSigterm(), ::extensions::ExitedOrKilled(CHILLOUT_EXIT_CODE, SIGTERM), "(::SigtermHandler|posixSignalHandler)");
}

// this test is disabled because you can catch c++ exceptions
TEST_F (DISABLED_RecoveryTest, ThrowExceptionTest) {
    ASSERT_EXIT(ThrowException(), ::testing::ExitedWithCode(CHILLOUT_EXIT_CODE), "");
}

TEST_F (DISABLED_RecoveryTest, MemoryTest) {
    ASSERT_EXIT(MemoryOverflow(), ::testing::ExitedWithCode(CHILLOUT_EXIT_CODE), "::NewHandler");
}

TEST_F (DISABLED_RecoveryTest, StackOverflowTest) {
    ASSERT_EXIT(StackOverflow(), ::testing::ExitedWithCode(CHILLOUT_EXIT_CODE), "::adfadf");
}

TEST_F (RecoveryTest, RaiseExceptionTest) {
    ASSERT_EXIT(RaiseSehException(), ::extensions::ExitedOrKilled(CHILLOUT_EXIT_CODE, SIGTERM), "Chillout SehHandler|posixSignalHandler");
}
