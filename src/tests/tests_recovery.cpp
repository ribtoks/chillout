#include <float.h>
#include <gtest/gtest.h>
#include <chillout.h>
#include <string>
#include "tests_source.h"

#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

#define WIDEN(quote) WIDEN2(quote)
#define WIDEN2(quote) L##quote

class RecoveryTest : public ::testing::Test
{
  protected:
    virtual void SetUp() {
        auto &chillout = Chillout::getInstance();
        chillout.init(L"chillout_test", WIDEN(STRINGIZE(CRASHDUMPSDIR)));
    }

    virtual void TearDown() {
        auto &chillout = Chillout::getInstance();
        chillout.deinit();
    }
};

TEST_F (RecoveryTest, PureVirtualMethodCallTest) {
    ASSERT_EXIT(Derived(), ::testing::ExitedWithCode(1), "");
}

TEST_F (RecoveryTest, AccessViolationTest) {
    ASSERT_EXIT(AccessViolation(), ::testing::ExitedWithCode(1), "");
}

TEST_F (RecoveryTest, InvalidParameterTest) {
    ASSERT_EXIT(InvalidParameter(), ::testing::ExitedWithCode(1), "");
}

TEST_F (RecoveryTest, SigillTest) {
    ASSERT_EXIT(RaiseSigill(), ::testing::ExitedWithCode(1), "");
}

TEST_F (RecoveryTest, SigsegvTest) {
    ASSERT_EXIT(RaiseSigsegv(), ::testing::ExitedWithCode(1), "");
}

TEST_F (RecoveryTest, SigtermTest) {
    ASSERT_EXIT(RaiseSigterm(), ::testing::ExitedWithCode(1), "");
}

// this test is disabled because you can catch c++ exceptions
// TEST_F (RecoveryTest, ThrowExceptionTest) {
//     ASSERT_EXIT(ThrowException(), ::testing::ExitedWithCode(1), "");
// }
