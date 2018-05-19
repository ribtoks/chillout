#include "chillout.h"

#ifdef _WIN32
#include "windows/windowscrashhandler.h"
#endif

void Chillout::init(const std::wstring &appName, const std::wstring &pathToDumpsDir)
{
    if (0 == m_InitCounter.fetch_add(1)) {
#ifdef _WIN32
        WindowsCrashHandler &handler = WindowsCrashHandler::getInstance();
        handler.Setup(appName, pathToDumpsDir);
#endif
    }
}

void Chillout::deinit() {
#ifdef _WIN32
    WindowsCrashHandler &handler = WindowsCrashHandler::getInstance();
    handler.Teardown();
#endif
}

void Chillout::setBacktraceCallback(const std::function<void(const char const *)> &callback) {
#ifdef _WIN32
    WindowsCrashHandler &handler = WindowsCrashHandler::getInstance();
    handler.SetBacktraceCallback(callback);
#endif
}

void Chillout::setCrashCallback(const std::function<void()> &callback) {
#ifdef _WIN32
    WindowsCrashHandler &handler = WindowsCrashHandler::getInstance();
    handler.SetCrashCallback(callback);
#endif
}
