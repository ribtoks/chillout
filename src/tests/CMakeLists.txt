cmake_minimum_required(VERSION 3.6)

project(chillout_tests_project)

set(SOURCES tests_main.cpp tests_source.h tests_source.cpp)

if(WIN32)
  set(SOURCES ${SOURCES} tests_crashes_win.cpp tests_recovery_win.cpp)
endif()

add_executable(chillout_test ${SOURCES})

target_include_directories(chillout_test PRIVATE ${CHILLOUT_SOURCE_DIR})
target_include_directories(chillout_test PRIVATE ${GTEST_SOURCE_DIR}/googletest/include)

# GTest needs threading support

# stackwalker stuff
target_link_libraries(chillout_test Advapi32 DbgHelp)

target_link_libraries(chillout_test gtest_main)
target_link_libraries(chillout_test chillout)

add_test(NAME ChilloutTest COMMAND chillout_test)
