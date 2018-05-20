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

static inline void walkStackTrace( const std::function<void(const char * const)> &callback, unsigned int max_frames = 127 )
{
    callback("stack trace:\n");
    char stackEntry[MAX_ENTRY_LENGTH];
    memset(&stackEntry[0], 0, MAX_ENTRY_LENGTH);

    // storage array for stack trace address data
    void* addrlist[max_frames+1];

    // retrieve current stack addresses
    unsigned int addrlen = backtrace( addrlist, sizeof( addrlist ) / sizeof( void* ));

    if ( addrlen == 0 ) 
    {
        callback( "  \n" );
        return;
    }

    // resolve addresses into strings containing "filename(function+address)",
    // Actually it will be ## program address function + offset
    // this array must be free()-ed
    char** symbollist = backtrace_symbols( addrlist, addrlen );

    size_t funcnamesize = 1024;
    char funcname[1024];

    // iterate over the returned symbol lines. skip the first, it is the
    // address of this function.
    for ( unsigned int i = 4; i < addrlen; i++ )
    {
        char* begin_name   = NULL;
        char* begin_offset = NULL;
        char* end_offset   = NULL;

        memset(&stackEntry[0], 0, MAX_ENTRY_LENGTH);

        // find parentheses and +address offset surrounding the mangled name
#ifdef DARWIN
        // OSX style stack trace
        for ( char *p = symbollist[i]; *p; ++p )
        {
            if (( *p == '_' ) && ( *(p-1) == ' ' ))
                begin_name = p-1;
            else if ( *p == '+' )
                begin_offset = p-1;
        }

        if ( begin_name && begin_offset && ( begin_name < begin_offset ))
        {
            *begin_name++ = '\0';
            *begin_offset++ = '\0';

            // mangled name is now in [begin_name, begin_offset) and caller
            // offset in [begin_offset, end_offset). now apply
            // __cxa_demangle():
            int status;
            char* ret = abi::__cxa_demangle( begin_name, &funcname[0],
                                             &funcnamesize, &status );
            if ( status == 0 ) 
            {
                funcname = ret; // use possibly realloc()-ed string
                snprintf(stackEntry, MAX_ENTRY_LENGTH, "  %-30s %-40s %s\n",
                         symbollist[i], funcname, begin_offset );
                callback(stackEntry);
            } else {
                // demangling failed. Output function name as a C function with
                // no arguments.
                snprintf( stackEntry, MAX_ENTRY_LENGTH, "  %-30s %-38s() %s\n",
                          symbollist[i], begin_name, begin_offset );
                callback(stackEntry);
            }

#else // !DARWIN - but is posix
      // not OSX style
      // ./module(function+0x15c) [0x8048a6d]
            for ( char *p = symbollist[i]; *p; ++p )
            {
                if ( *p == '(' )
                    begin_name = p;
                else if ( *p == '+' )
                    begin_offset = p;
                else if ( *p == ')' && ( begin_offset || begin_name ))
                    end_offset = p;
            }

            if ( begin_name && end_offset && ( begin_name < end_offset ))
            {
                *begin_name++   = '\0';
                *end_offset++   = '\0';
                if ( begin_offset )
                    *begin_offset++ = '\0';

                // mangled name is now in [begin_name, begin_offset) and caller
                // offset in [begin_offset, end_offset). now apply
                // __cxa_demangle():

                int status = 0;
                char* ret = abi::__cxa_demangle( begin_name, funcname,
                                                 &funcnamesize, &status );
                char* fname = begin_name;
                if ( status == 0 ) 
                    fname = ret;

                if ( begin_offset )
                {
                    snprintf( stackEntry, MAX_ENTRY_LENGTH, "  %-30s ( %-40s	+ %-6s)	%s\n",
                              symbollist[i], fname, begin_offset, end_offset );
                    callback(stackEntry);
                } else {
                    snprintf( stackEntry, MAX_ENTRY_LENGTH, "  %-30s ( %-40s	  %-6s)	%s\n",
                              symbollist[i], fname, "", end_offset );
                    callback(stackEntry);
                }
#endif  // !DARWIN - but is posix
            } else {
                // couldn't parse the line? print the whole line.
                snprintf(stackEntry, MAX_ENTRY_LENGTH, "  %-40s\n", symbollist[i]);
                callback(stackEntry);
            }
        }

        free(symbollist);
    }

    void crashHandler( int signum, siginfo_t* si, void* ucontext ) {
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
        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = crashHandler;
        sigemptyset( &sa.sa_mask );
    
        sigaction( SIGABRT, &sa, NULL );
        sigaction( SIGSEGV, &sa, NULL );
        sigaction( SIGBUS,  &sa, NULL );
        sigaction( SIGILL,  &sa, NULL );
        sigaction( SIGFPE,  &sa, NULL );
        sigaction( SIGPIPE, &sa, NULL );
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
