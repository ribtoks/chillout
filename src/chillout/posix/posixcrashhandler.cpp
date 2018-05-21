#include "posixcrashhandler.h"

#include <execinfo.h>
#include <errno.h>
#include <dlfcn.h>
#include <cxxabi.h>
#include <cstdio>
#include <cstdlib>
#include <signal.h>
#include <cstring>
#include <cstdint>

#include <memory>

#include "../defines.h"

struct FreeDeleter {
    void operator()(void* ptr) const {
        free(ptr);
    }
};

char *fake_alloc(char **memory, size_t size) {
    char *allocated = *memory;
    char *last = allocated + size;
    *last = '\0';
    *memory += size + 1;
    return allocated;
}

char *dlDemangle(void *addr, char *symbol, int frameIndex, char *memory) {
    Dl_info info;
    if (dladdr(addr, &info) != 0) {
        const int stackFrameSize = 4096;
        char *stackFrame = fake_alloc(&memory, stackFrameSize);
                    
        if ((info.dli_sname != NULL) && (info.dli_sname[0] == '_')) {
            int status = -1;
            std::unique_ptr<char, FreeDeleter> demangled(abi::__cxa_demangle(info.dli_sname, NULL, 0, &status));
            snprintf(stackFrame, stackFrameSize, "%-3d %*p %s + %zd\n",
                     frameIndex, int(2 + sizeof(void*) * 2), addr,
                     status == 0 ? demangled.get() :
                     info.dli_sname == 0 ? symbol : info.dli_sname,
                     (char *)addr - (char *)info.dli_saddr);

        } else {
            snprintf(stackFrame, stackFrameSize, "%-3d %*p %s\n",
                     frameIndex, int(2 + sizeof(void*) * 2), addr, symbol);
        }

        return stackFrame;
    }

    return NULL;
}

void walkStackTrace(const std::function<void(const char * const)> &callback, char *memory, size_t memorySize, unsigned int maxFrames = 127) {
    const size_t framesSize = maxFrames * sizeof(void*);
    void **callstack = reinterpret_cast<void**>(fake_alloc(&memory, framesSize));
    int frames = backtrace(callstack, maxFrames);

    const int stackOffset = callstack[2] == callstack[1] ? 2 : 1;

    std::unique_ptr<char*, FreeDeleter> symbolsPtr(backtrace_symbols(callstack, frames));
    if (!symbolsPtr) { return; }
    
    char **symbols = symbolsPtr.get();

    for (int i = stackOffset; i < frames; ++i) {
        memset(memory, 0, memorySize - framesSize - 1);

        char *stackFrame = dlDemangle(callstack[i], symbols[i], i, memory);
        if (stackFrame) {
            callback(stackFrame);
        }
    }

    if (frames == maxFrames) {
        callback("[truncated]\n");
    }
}

void posixSignalHandler( int signum, siginfo_t* si, void* ucontext ) {
    (void)si;
    (void)ucontext;

    auto &handler = PosixCrashHandler::getInstance();
    handler.handleCrash();

    // If you caught one of the above signals, it is likely you just 
    // want to quit your program right now.
    //exit( signum );
    exit(CHILLOUT_EXIT_CODE);
}

PosixCrashHandler::PosixCrashHandler() {
    memset(&m_Memory[0], 0, sizeof(m_Memory));
}

void PosixCrashHandler::setup() {
    struct sigaction sa;
    sa.sa_sigaction = posixSignalHandler;
    sigemptyset( &sa.sa_mask );

#ifdef __APPLE__
    /* for some reason we backtrace() doesn't work on osx
       when we use an alternate stack */
    sa.sa_flags = SA_SIGINFO;
#else
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
#endif
    
    sigaction( SIGABRT, &sa, NULL );
    sigaction( SIGSEGV, &sa, NULL );
    sigaction( SIGBUS,  &sa, NULL );
    sigaction( SIGILL,  &sa, NULL );
    sigaction( SIGFPE,  &sa, NULL );
    sigaction( SIGPIPE, &sa, NULL );
    sigaction( SIGTERM, &sa, NULL );
}

void PosixCrashHandler::teardown() {
    struct sigaction sa;
    sigset_t mysigset;
        
    sigemptyset(&mysigset);
        
    sa.sa_handler = SIG_DFL;
    sa.sa_mask = mysigset;
    sa.sa_flags = 0;

    sigaction( SIGABRT, &sa, NULL );
    sigaction( SIGSEGV, &sa, NULL );
    sigaction( SIGBUS,  &sa, NULL );
    sigaction( SIGILL,  &sa, NULL );
    sigaction( SIGFPE,  &sa, NULL );
    sigaction( SIGPIPE, &sa, NULL );
    sigaction( SIGTERM, &sa, NULL );
}

void PosixCrashHandler::handleCrash() {
    if (m_BacktraceCallback) {
        walkStackTrace(m_BacktraceCallback, m_Memory, sizeof(m_Memory));
    }

    if (m_CrashCallback) {
        m_CrashCallback();
    }
}
    
void PosixCrashHandler::setCrashCallback(const std::function<void()> &callback) {
    m_CrashCallback = callback;
}
    
void PosixCrashHandler::setBacktraceCallback(const std::function<void(const char * const)> &callback) {
    m_BacktraceCallback = callback;
}
