cmake_minimum_required(VERSION 3.1..3.29)

project(
    vgmchips_opll
    LANGUAGES CXX
)

add_library(
    opll_wrapper STATIC
    opll_wrapper.cpp opll_wrapper.h
)

target_include_directories(opll_wrapper PUBLIC .)
target_include_directories(opll_wrapper PUBLIC ../../vgmcore/include)

add_subdirectory(emu2413)
target_link_libraries(opll_wrapper PRIVATE emu2413)