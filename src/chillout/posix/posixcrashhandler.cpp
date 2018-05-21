#include "posixcrashhandler.h"

#include <execinfo.h>
#include <errno.h>
#include <cxxabi.h>
#include <cxxabi.h>
#include <cstdio>
#include <cstdlib>
#include <signal.h>
#include <cstring>
#include <cstdint>

#include <algorithm>

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
bool demangleLine(char *line, char *memory) {
    char *functionSymbol = fake_alloc(&memory, 1024);
    char *moduleName = fake_alloc(&memory, 1024);
    int offset = 0;
    char *addr = fake_alloc(&memory, 64);

    // split the string, take out chunks out of stack trace
    // we are primarily interested in module, function and address
    sscanf(strs[i], "%*s %s %s %s %*s %d", moduleName, addr, functionSymbol, &offset);

    int validCppName = 0;
    //  if this is a C++ library, symbol will be demangled
    //  on success function returns 0
    std::unique_ptr<char, FreeDeleter> functionName(abi::__cxa_demangle(functionSymbol, NULL, 0, &validCppName));
    
    char *stackFrame = fake_alloc(&memory, 4096);
    if (validCppName == 0) {
        // success
        sprintf(stackFrame, "(%s)\t0x%s — %s + %d\n", moduleName, addr, functionName, offset);
    } else {
        //  in the above traceback (in comments) last entry is not
        //  from C++ binary, last frame, libdyld.dylib, is printed
        //  from here
        sprintf(stackFrame, "(%s)\t0x%s — %s + %d\n",
                moduleName, addr, functionName, offset);
    }

    return true;
}
#else
bool demangleLine(char *line, char *memory) {
    return false;
}
#endif

void walkStackTrace(const std::function<void(const char * const)> &callback, char *memory, unsigned int maxFrames = 127) {
    const size_t framesSize = (maxFrames + 2) * sizeof(void*);
    void **trace = reinterpret_cast<void**>(fake_alloc(&memory, framesSize));
    int frames = backtrace(trace, maxFrames + 2);

    const int stackOffset = trace[2] == trace[1] ? 2 : 1;

    std::unique_ptr<char*, FreeDeleter> symbols(backtrace_symbols(trace, frames));
    if (!symbols) { return; }

    for (int i = stackOffset; i < frames; ++i) {
        char* traceLine = symbols.get()[i];
        if (demangleLine(traceLine, memory)) {
            callback(memory);
        } else {
            callback(traceLine);
        }
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
        walkStackTrace(m_BacktraceCallback, m_Memory);
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


