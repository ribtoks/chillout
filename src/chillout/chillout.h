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
    void setBacktraceCallback(const std::function<void(const char const *)> &callback);

private:
    Chillout() {}
    Chillout(Chillout const&);
    void operator=(Chillout const&);

private:
    //std::function<void()> m_CrashHandler;
    //std::function<void(const char const *)> m_BacktraceCallback;
    std::atomic_int m_InitCounter = 0;
};

#endif // CHILLOUT_H
