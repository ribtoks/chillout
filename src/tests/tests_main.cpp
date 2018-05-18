#include <gtest/gtest.h>

#include <crtdbg.h>
#include <errno.h>

int main(int argc, char **argv) {
    // _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    // _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    // _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    // _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    // _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    // _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
