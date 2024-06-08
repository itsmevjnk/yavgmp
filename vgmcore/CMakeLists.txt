cmake_minimum_required(VERSION 3.1..3.29)

project(
    vgmcore
    LANGUAGES CXX
)

add_library(
    vgmcore SHARED
    src/vgm_parser.cpp src/vgm_header.cpp src/gd3.cpp
    include/vgm_parser.h include/vgm_header.h include/gd3.h
)

target_include_directories(vgmcore PUBLIC include)

add_subdirectory(zstr)
target_link_libraries(vgmcore PRIVATE zstr::zstr)