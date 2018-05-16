cmake_minimum_required(VERSION 3.6)

project(CHILLOUT)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_STANDARD_REQUIRED on)

enable_testing()

add_subdirectory(src/chillout)
add_subdirectory(src/tests)
