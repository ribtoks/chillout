cmake_minimum_required(VERSION 3.6)

project(chillout CXX)

set(SOURCES chillout.h chillout.cpp)

if(WIN32)
  set(SOURCES ${SOURCES} windows/StackWalker.h windows/StackWalker.cpp windows/windowscrashhandler.h windows/windowscrashhandler.cpp)
#  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

add_library(chillout SHARED STATIC ${SOURCES})
install(TARGETS chillout DESTINATION lib)
