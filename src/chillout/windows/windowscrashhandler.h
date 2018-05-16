#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <new.h>
#include <signal.h>
#include <exception>
#include <sys/stat.h>
#include <psapi.h>
#include <rtcapi.h>
#include <Shellapi.h>
#include <dbghelp.h>

#include <functional>
#include <mutex>
#include <wstring>

struct ThreadExceptionHandlers
{
    ThreadExceptionHandlers()
    {
        m_prevTerm = NULL;
        m_prevUnexp = NULL;
        m_prevSigFPE = NULL;
        m_prevSigILL = NULL;
        m_prevSigSEGV = NULL;
    }

    terminate_handler m_prevTerm;        // Previous terminate handler   
    unexpected_handler m_prevUnexp;      // Previous unexpected handler
    void (__cdecl *m_prevSigFPE)(int);   // Previous FPE handler
    void (__cdecl *m_prevSigILL)(int);   // Previous SIGILL handler
    void (__cdecl *m_prevSigSEGV)(int);  // Previous illegal storage access handler
};

// code mostly from https://www.codeproject.com/articles/207464/WebControls/
class WindowsCrashHandler
{
public:
    enum CrashDumpSize {
        CrashDumpSmall,
        CrashDumpNormal,
        CrashDumpFull
    };
    
public:
    static WindowsCrashHandler& getInstance()
    {
        static WindowsCrashHandler instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    
private:
    WindowsCrashHandler();

public:
    void Setup(const std::wstring &appName, const std::wstring &dumpsDir);
    void SetCrashDumpSize(CrashDumpSize size);
    void SetCrashCallback(const std::function<void()> &crashCallback);
    void SetBacktraceCallback(const std::function<void(const char const *)> &backtraceCallback);
    void HandleCrash(EXCEPTION_POINTERS* pExPtrs);

private:
    void Backtrace(EXCEPTION_POINTERS* pExPtrs);
    void CreateDump(EXCEPTION_POINTERS* pExPtrs);
    bool IsDataSectionNeeded(const WCHAR* pModuleName);

public:
    // Sets exception handlers that work on per-process basis
    void SetProcessExceptionHandlers();
    void UnsetProcessExceptionHandlers();

    // Installs C++ exception handlers that function on per-thread basis
    int SetThreadExceptionHandlers();
    int UnsetThreadExceptionHandlers();

    /* Exception handler functions. */

    static void __cdecl TerminateHandler();
    static void __cdecl UnexpectedHandler();

    static void __cdecl PureCallHandler();

    static void __cdecl InvalidParameterHandler(const wchar_t* expression,
        const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved);

    static int __cdecl NewHandler(size_t);

    static void SigabrtHandler(int);
    static void SigfpeHandler(int /*code*/, int subcode);
    static void SigintHandler(int);
    static void SigillHandler(int);
    static void SigsegvHandler(int);
    static void SigtermHandler(int);

private:
    std::function<void()> m_CrashCallback;
    std::function<void(const char const *)> m_BacktraceCallback;
    std::mutex m_CrashMutex;
    CrashDumpSize m_CrashDumpSize;
    std::wstring m_AppName;
    std::wstring m_DumpsDir;
    
    std::map<DWORD, ThreadExceptionHandlers> m_ThreadExceptionHandlers;
    std::mutex m_ThreadHandlersMutex;

    // Previous SEH exception filter.
    LPTOP_LEVEL_EXCEPTION_FILTER  m_oldSehHandler;  

#if _MSC_VER>=1300
    _purecall_handler m_prevPurec;   // Previous pure virtual call exception filter.
    _PNH m_prevNewHandler; // Previous new operator exception filter.
#endif

#if _MSC_VER>=1400
    _invalid_parameter_handler m_prevInvpar; // Previous invalid parameter exception filter.
#endif

#if _MSC_VER>=1300 && _MSC_VER<1400
    _secerr_handler_func m_prevSec; // Previous security exception filter.
#endif

    void (__cdecl *m_prevSigABRT)(int); // Previous SIGABRT handler.  
    void (__cdecl *m_prevSigINT)(int);  // Previous SIGINT handler.
    void (__cdecl *m_prevSigTERM)(int); // Previous SIGTERM handler.
};

#endif // CRASHHANDLER_H
