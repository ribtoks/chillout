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

#include "../defines.h"

#define MAX_ENTRY_LENGTH 2048

static inline void walkStackTrace( const std::function<void(const char * const)> &callback, unsigned int maxFrames = 127 )
{
    int callstack[maxFrames + 1];
    int frames = backtrace((void**) callstack, maxFrames);
    
    // get the human-readable symbols (mangled)
    char** strs = backtrace_symbols((void**) callstack, frames);
    for (int i = 0; i < frames; ++i) {
        char functionSymbol[1024] = {};
        char moduleName[1024] = {};
        int  offset = 0;
        char addr[48] = {};

        // split the string, take out chunks out of stack trace
        // we are primarily interested in module, function and address
        sscanf(strs[i], "%*s %s %s %s %*s %d", &moduleName[0], &addr[0], &functionSymbol[0], &offset);

        int validCppName = 0;
        //  if this is a C++ library, symbol will be demangled
        //  on success function returns 0
        char* functionName = abi::__cxa_demangle(functionSymbol, NULL, 0, &validCppName);

        char stackFrame[4096] = {};
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

        if (functionName) {
            free(functionName);
        }

        callback(stackFrame);
    }

    free(strs);
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
        walkStackTrace(m_BacktraceCallback);
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
