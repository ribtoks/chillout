#ifndef POSIXCRASHHANDLER_H
#define POSIXCRASHHANDLER_H

#include <functional>

// code mosty from https://oroboro.com/stack-trace-on-crash/
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
    void setup();
    void teardown();
    void handleCrash();
    void setCrashCallback(const std::function<void()> &callback);
    void setBacktraceCallback(const std::function<void(const char * const)> &callback);

private:
    std::function<void()> m_CrashCallback;
    std::function<void(const char * const)> m_BacktraceCallback;
};

#endif // POSIXCRASHHANDLER_H









