QT       -= core gui

TARGET = chillout
TEMPLATE = lib
CONFIG += c++11
CONFIG += staticlib

HEADERS += \
    chillout.h \
    defines.h \
    common/common.h

SOURCES += \
    chillout.cpp \
    common/common.cpp

win32 {
    LIBS += -lAdvapi32 -lDbgHelp
    DEFINES += \
        _UNICODE \
        _MBCS

    HEADERS += \
        windows/StackWalker.h \
        windows/windowscrashhandler.h

    SOURCES += \
        windows/StackWalker.cpp \
        windows/windowscrashhandler.cpp
}

unix {
    LIBS += -ldl

    HEADERS += \
        posix/posixcrashhandler.h

    SOURCES += \
        posix/posixcrashhandler.cpp
}

DISTFILES += \
    chillout.pro.user
