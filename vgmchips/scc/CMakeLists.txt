cmake_minimum_required(VERSION 3.1..3.29)

project(
    vgmchips_scc
    LANGUAGES CXX
)

add_library(
    scc_wrapper STATIC
    scc_wrapper.cpp scc_wrapper.h
)

target_include_directories(scc_wrapper PUBLIC .)
target_include_directories(scc_wrapper PUBLIC ../../vgmcore/include)

add_subdirectory(emu2212)
target_link_libraries(scc_wrapper PRIVATE emu2212)