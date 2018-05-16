#include <float.h>
#include <gtest/gtest.h>
#include "tests_source.h"

TEST (CrashTest, PureVirtualMethodCallTest) {
    ASSERT_DEATH(Derived(), "");
}

TEST (CrashTest, AccessViolationTest) {
    ASSERT_DEATH(AccessViolation(), "");
}
