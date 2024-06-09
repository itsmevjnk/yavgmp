cmake_minimum_required(VERSION 3.1..3.29)

project(
    vgmchips_psg
    LANGUAGES CXX
)

add_library(
    psg_native STATIC
    psg_native.cpp psg_native.h
)

target_include_directories(psg_native PUBLIC .)
target_include_directories(psg_native PUBLIC ../../vgmcore/include)

add_library(
    psg_wrapper STATIC
    psg_wrapper.cpp psg_wrapper.h
)

target_include_directories(psg_wrapper PUBLIC .)
target_include_directories(psg_wrapper PUBLIC ../../vgmcore/include)

add_subdirectory(emu76489)
target_link_libraries(psg_wrapper PRIVATE emu76489)