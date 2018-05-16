#include "chillout.h"

#ifdef _WIN32
#include "windows/StackWalker.h"

#endif


void Chillout::init(const std::function<void()> &handler,
                    const std::function<void(const char const *)> &callback)
{
    m_CrashHandler = handler;
    m_StackEntryCallback = callback;
}

