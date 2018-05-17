#include <float.h>
#include <gtest/gtest.h>
#include "tests_source.h"

TEST (CrashTest, PureVirtualMethodCallTest) {
    ASSERT_DEATH(Derived(), "");
}

TEST (CrashTest, AccessViolationTest) {
    ASSERT_DEATH(AccessViolation(), "");
}

TEST (CrashTest, InvalidParameterTest) {
    ASSERT_DEATH(InvalidParameter(), "");
}

TEST (CrashTest, SigillTest) {
    ASSERT_DEATH(RaiseSigill(), "");
}

TEST (CrashTest, SigsegvTest) {
    ASSERT_DEATH(RaiseSigsegv(), "");
}

TEST (CrashTest, SigtermTest) {
    ASSERT_DEATH(RaiseSigterm(), "");
}

TEST (CrashTest, ThrowExceptionTest) {
    ASSERT_DEATH(ThrowException(), "");
}
