#include <float.h>
#include <gtest/gtest.h>
#include <chillout.h>
#include "tests_source.h"

class RecoveryTest : public ::testing::Test
{
  protected:
    virtual void SetUp() {
        auto &chillout = Chillout::getInstance();
        chillout.init(L"chillout_test", L".");
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

