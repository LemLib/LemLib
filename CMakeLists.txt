# Copied mostly from Ryan4253/RaidZeroLib: https://github.com/Ryan4253/RaidZeroLib/blob/06cc9796c5e7a1d8651d2b725cfde4c0e34d329f/CMakeLists.txt
cmake_minimum_required(VERSION 3.6)
project(LemLib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wnull-dereference -Wno-psabi -Wno-unused-function -pthread -g -O0 -fprofile-arcs -ftest-coverage -D THREADS_STD")

enable_testing()

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in external-dependencies/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external-dependencies )
if(result)
    message(FATAL_ERROR "CMake step for dependencies failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external-dependencies )
if(result)
    message(FATAL_ERROR "Build step for dependencies failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

include_directories(include)

add_executable(LemLib
  include/lemlib/pose.hpp

  src/lemlib/pose.cpp

  test/pose.cpp
)

# Link against gtest
target_link_libraries(LemLib gtest_main)

include(GoogleTest)
gtest_discover_tests(LemLib)