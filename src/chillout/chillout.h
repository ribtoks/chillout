#ifndef CHILLOUT_H
#define CHILLOUT_H

#include <functional>

class Chillout {
public:
    static Chillout& getInstance()
    {
        static Chillout instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

public:
    void init(const std::function<void()> &handler,
              const std::function<void(const char const *)> &stackEntryCallback);

private:
    Chillout(Chillout const&);
    void operator=(Chillout const&);

private:
    std::function<void()> m_CrashHandler;
    std::function<void(const char const *)> m_StackEntryCallback;
};

#endif // CHILLOUT_H
