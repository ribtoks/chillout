# chillout

[![Build status](https://ci.appveyor.com/api/projects/status/b2mfobskucq936hc/branch/master?svg=true)](https://ci.appveyor.com/project/Ribtoks/chillout/branch/master)
[![Build Status](https://travis-ci.org/ribtoks/chillout.svg?branch=master)](https://travis-ci.org/ribtoks/chillout)

![os windows](https://img.shields.io/badge/OS-Windows-yellowgreen.svg)
![os mac](https://img.shields.io/badge/OS-macOS-yellowgreen.svg)
![os linux](https://img.shields.io/badge/OS-Linux-yellowgreen.svg)

![license](https://img.shields.io/badge/license-MIT-blue.svg)
![copyright](https://img.shields.io/badge/%C2%A9-Taras_Kushnir-blue.svg)
![language](https://img.shields.io/badge/language-c++-blue.svg)
![c++](https://img.shields.io/badge/std-c++11-blue.svg) 

Chillout is a **work-in-progress** simple cross-platform crash handling library.

The idea is to enable developers of native desktop applications to gracefully handle crashes. This library can be used to catch various unhandled exceptions (access violation, signals, alloc errors etc.), generate backtrace for logs and possibly crash dump (the latter currently only for Windows).

### Basic usage

    auto &chillout = Chillout::getInstance();
    // install various crash handlers
    chillout.init(L"My App Name");
    
    chillout.setBacktraceCallback([](const char * const stackEntry) {
        fprintf(stderr, "my trace:  %s", stackEntry);
    });
    
    chillout.setCrashCallback([]() {
        // restart your app / save dump / etc
    });

### Disclaimer

Yes, ~Breakpad~ Crashpad exists, but sometimes it is too big or too complicated to setup (e.g. for Qt project). This library is definitely less reliable and featured than age-tested Breakpad/Crashpad, but it is simple, small and covered by tests.
