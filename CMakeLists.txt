cmake_minimum_required(VERSION 3.10)
project(HorrifiedGame)

set(CMAKE_CXX_STANDARD 17)

# اضافه کردن مسیرهای هدر
include_directories(
    include
    Monsters
)

# جمع‌آوری همه فایل‌های cpp
file(GLOB_RECURSE SRC_FILES
    src/*.cpp
    Monsters/*.cpp
)

add_executable(HorrifiedGame ${SRC_FILES})

