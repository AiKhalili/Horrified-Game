cmake_minimum_required(VERSION 3.15)
project(Horrified)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(
    include
    include/app
    include/audio
    include/core
    include/graphics
    include/graphics/ui
    include/saves
)

file(GLOB_RECURSE SRC_FILES
    src/*.cpp
    main.cpp
)

find_package(raylib REQUIRED)

add_executable(Horrified ${SRC_FILES})
target_link_libraries(Horrified raylib)

add_custom_target(copy_assets ALL
 COMMAND ${CMAKE_COMMAND} -E copy_directory
 ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
 COMMENT "Copying assets folder..."
)

add_dependencies(Horrified copy_assets)