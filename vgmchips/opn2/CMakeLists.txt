cmake_minimum_required(VERSION 3.1..3.29)

project(
    vgmchips_opn2
    LANGUAGES CXX
)

add_library(
    opn2_wrapper STATIC
    opn2_wrapper.cpp opn2_wrapper.h
)

target_include_directories(opn2_wrapper PUBLIC .)
target_include_directories(opn2_wrapper PUBLIC ../../vgmcore/include)

add_subdirectory(nukedopn2)
target_link_libraries(opn2_wrapper PRIVATE nukedopn2)