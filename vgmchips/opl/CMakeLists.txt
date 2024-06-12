cmake_minimum_required(VERSION 3.1..3.29)

project(
    vgmchips_opl
    LANGUAGES CXX
)

add_library(
    opl_wrapper STATIC
    opl_wrapper.cpp opl_wrapper.h
)

target_include_directories(opl_wrapper PUBLIC .)
target_include_directories(opl_wrapper PUBLIC ../../vgmcore/include)

add_subdirectory(emu8950)
target_link_libraries(opl_wrapper PRIVATE emu8950)