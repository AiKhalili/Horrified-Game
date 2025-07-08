cmake_minimum_required(VERSION 3.10)
project(HorrifiedGame)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(HorrifiedGame ${SOURCES})

target_include_directories(HorrifiedGame PRIVATE include)

