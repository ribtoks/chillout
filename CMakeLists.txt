cmake_minimum_required(VERSION 3.6)

project(chillout CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(GTEST_SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendors/gtest)
set(CHILLOUT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/chillout)

if (MSVC)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING")
   set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif (MSVC)

#add_definitions(-DGTEST_HAS_SEH=0)

enable_testing()

add_subdirectory(src/chillout)
add_subdirectory(vendors/gtest)
add_subdirectory(src/tests)
