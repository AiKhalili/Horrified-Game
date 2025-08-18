cmake_minimum_required(VERSION 3.15)
project(Horrified)

# فعال کردن C++17
set(CMAKE_CXX_STANDARD 17)

# تعریف نوع ساخت؛ اگر چیزی تعریف نشده، Debug پیش‌فرض شود
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# تنظیم گزینه‌های کامپایل برای دیباگ
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# مسیر خروجی باینری
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# اضافه کردن مسیر هدرها
include_directories(
    include
    include/app
    include/audio
    include/core
    include/graphics
    include/graphics/ui
    include/saves
)

# فایل‌های cpp
file(GLOB_RECURSE SRC_FILES
    src/*.cpp
    main.cpp
)

# پیدا کردن raylib
find_package(raylib REQUIRED)

add_executable(Horrified ${SRC_FILES})
target_link_libraries(Horrified raylib)

# ساخت تارگت سفارشی که همیشه اجرا میشه
add_custom_target(copy_assets ALL
 COMMAND ${CMAKE_COMMAND} -E copy_directory
 ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
 COMMENT "Copying assets folder..."
)

# اطمینان از اینکه قبل از ساخت اجرایی اجرا میشه
add_dependencies(Horrified copy_assets)