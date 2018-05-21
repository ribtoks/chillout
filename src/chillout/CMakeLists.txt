cmake_minimum_required(VERSION 3.6)

project(chillout CXX)

set(SOURCES chillout.h chillout.cpp defines.h)

if(WIN32)
  set(SOURCES ${SOURCES} windows/StackWalker.h windows/StackWalker.cpp windows/windowscrashhandler.h windows/windowscrashhandler.cpp)
else()
  set(SOURCES ${SOURCES} posix/posixcrashhandler.h posix/posixcrashhandler.cpp)
endif()

message("Debug flags are: ${CMAKE_CXX_FLAGS_DEBUG}")

add_library(chillout SHARED STATIC ${SOURCES})

if(UNIX)
  target_link_libraries(chillout dl)
endif()


install(TARGETS chillout DESTINATION lib)
