#include "chillout.h"

#ifdef _WIN32
#include "windows/windowscrashhandler.h"
#else
#include "posix/posixcrashhandler.h"
#endif

namespace Debug {
    void Chillout::init(const string_t &appName, const string_t &pathToDumpsDir) {
        if (0 == m_InitCounter.fetch_add(1)) {
#ifdef _WIN32
            WindowsCrashHandler &handler = WindowsCrashHandler::getInstance();
            handler.setup(appName, pathToDumpsDir);
#else
            PosixCrashHandler &handler = PosixCrashHandler::getInstance();
            handler.setup(appName, pathToDumpsDir);
#endif
        }
    }

    void Chillout::deinit() {
#ifdef _WIN32
        WindowsCrashHandler &handler = WindowsCrashHandler::getInstance();
        handler.teardown();
#else
        PosixCrashHandler &handler = PosixCrashHandler::getInstance();
        handler.teardown();
#endif
    }

    void Chillout::setBacktraceCallback(const std::function<void(const char * const)> &callback) {
#ifdef _WIN32
        WindowsCrashHandler &handler = WindowsCrashHandler::getInstance();
        handler.setBacktraceCallback(callback);
#else
        PosixCrashHandler &handler = PosixCrashHandler::getInstance();
        handler.setBacktraceCallback(callback);
#endif
    }

    void Chillout::setCrashCallback(const std::function<void()> &callback) {
#ifdef _WIN32
        WindowsCrashHandler &handler = WindowsCrashHandler::getInstance();
        handler.setCrashCallback(callback);
#else
        PosixCrashHandler &handler = PosixCrashHandler::getInstance();
        handler.setCrashCallback(callback);
#endif
    }
}
