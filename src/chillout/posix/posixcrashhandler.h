#ifndef POSIXCRASHHANDLER_H
#define POSIXCRASHHANDLER_H

#include <functional>
#include <string>

namespace Debug {
    class PosixCrashHandler {
    public:
        static PosixCrashHandler& getInstance()
        {
            static PosixCrashHandler instance; // Guaranteed to be destroyed.
            // Instantiated on first use.
            return instance;
        }

    private:
        PosixCrashHandler();

    public:
        void setup(const std::string &appName, const std::string &crashDirPath);
        void teardown();
        void handleCrash();
        void setCrashCallback(const std::function<void()> &callback);
        void setBacktraceCallback(const std::function<void(const char * const)> &callback);

    private:
        std::function<void()> m_crashCallback;
        std::function<void(const char * const)> m_backtraceCallback;
        char m_memory[10*1024];
        std::string m_backtraceFilePath;
    };
}

#endif // POSIXCRASHHANDLER_H
