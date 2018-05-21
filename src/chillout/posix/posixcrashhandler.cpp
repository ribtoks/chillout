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

#define MAX_ENTRY_LENGTH 2048

const char s_MangledSymbolPrefix[] = "_Z";
const char s_MangledSymbolCharacters[] = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

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

#ifdef __APPLE__
char *demangleLine(char *line, char *memory) {
    // 1   chillout_test                       0x0000000104d34e6e _ZN17PosixCrashHandler11handleCrashEv + 78
    
    char *functionSymbol = fake_alloc(&memory, 1024);
    char *moduleName = fake_alloc(&memory, 1024);
    int offset = 0;
    char *addr = fake_alloc(&memory, 64);

    // split the string, take out chunks out of stack trace
    // we are primarily interested in module, function and address
    sscanf(line, "%*s %s %s %s %*s %d", moduleName, addr, functionSymbol, &offset);

    int validCppName = 0;
    //  if this is a C++ library, symbol will be demangled
    //  on success function returns 0
    std::unique_ptr<char, FreeDeleter> functionName(abi::__cxa_demangle(functionSymbol, NULL, 0, &validCppName));
    
    char *stackFrame = fake_alloc(&memory, 4096);
    if (validCppName == 0) {
        // success
        sprintf(stackFrame, "(%s)\t0x%s — %s + %d\n", moduleName, addr, functionName.get(), offset);
    } else {
        //  in the above traceback (in comments) last entry is not
        //  from C++ binary, last frame, libdyld.dylib, is printed
        //  from here
        sprintf(stackFrame, "(%s)\t0x%s — %s + %d\n",
                moduleName, addr, functionName.get(), offset);
    }

    return stackFrame;
}
#else
char *demangleLine(char *line, char *memory) {
    // /home/travis/build/ribtoks/chillout/build/src/tests/chillout_test() [0x40e274]
    // /lib/x86_64-linux-gnu/libc.so.6(gsignal+0x37)
    return nullptr;
}
#endif

void walkStackTrace(const std::function<void(const char * const)> &callback, char *memory, size_t memorySize, unsigned int maxFrames = 127) {
    const size_t framesSize = maxFrames * sizeof(void*);
    void **callstack = reinterpret_cast<void**>(fake_alloc(&memory, framesSize));
    int frames = backtrace(callstack, maxFrames);

    const int stackOffset = callstack[2] == callstack[1] ? 2 : 1;

    std::unique_ptr<char*, FreeDeleter> symbolsPtr(backtrace_symbols(callstack, frames));
    if (!symbolsPtr) { return; }
    
    char **symbols = symbolsPtr.get();
    const int stackFrameSize = 4096;

    for (int i = stackOffset; i < frames; ++i) {
        //char* traceLine = symbols.get()[i];
        Dl_info info;
        if (dladdr(callstack[i], &info) && info.dli_sname) {
            int status = -1;
            memset(memory, 0, memorySize - framesSize);
            char *stackFrame = fake_alloc(&memory, stackFrameSize);
                    
            if (info.dli_sname[0] == '_') {
                std::unique_ptr<char, FreeDeleter> demangled(abi::__cxa_demangle(info.dli_sname, NULL, 0, &status));
                snprintf(stackFrame, stackFrameSize, "%-3d %*p %s + %zd\n",
                         i, int(2 + sizeof(void*) * 2), callstack[i],
                         status == 0 ? demangled.get() :
                         info.dli_sname == 0 ? symbols[i] : info.dli_sname,
                         (char *)callstack[i] - (char *)info.dli_saddr);

            } else {
                snprintf(stackFrame, stackFrameSize, "%-3d %*p %s\n",
                         i, int(2 + sizeof(void*) * 2), callstack[i], symbols[i]);
            }

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


