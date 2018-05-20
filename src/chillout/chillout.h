#ifndef CHILLOUT_H
#define CHILLOUT_H

#include <functional>
#include <string>
#include <atomic>

class Chillout {
public:
    static Chillout& getInstance()
    {
        static Chillout instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

public:
    void init(const std::wstring &appName, const std::wstring &pathToDumpsDir);
    void deinit();
    void setBacktraceCallback(const std::function<void(const char * const)> &callback);
    void setCrashCallback(const std::function<void()> &callback);

private:
    Chillout(): m_InitCounter(0) {}
    Chillout(Chillout const&);
    void operator=(Chillout const&);

private:
    std::atomic_int m_InitCounter;
};

#endif // CHILLOUT_H
