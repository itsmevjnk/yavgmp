cmake_minimum_required(VERSION 3.1..3.29)

project(
    vgmchips_rf5c
    LANGUAGES CXX
)

add_library(
    rf5c STATIC
    rf5c.cpp rf5c.h
)

target_include_directories(rf5c PUBLIC .)
target_include_directories(rf5c PUBLIC ../../vgmcore/include)
