cmake_minimum_required(VERSION 3.1..3.29)

project(
    vgmcore
    LANGUAGES CXX
)

add_library(
    vgmcore SHARED
    src/vgm.cpp src/vgm_file.cpp src/vgm_header.cpp
    include/vgm.h include/vgm_file.h include/vgm_header.h
)

target_include_directories(vgmcore PUBLIC include)

find_package(ZLIB REQUIRED)
target_link_libraries(vgmcore ZLIB::ZLIB)