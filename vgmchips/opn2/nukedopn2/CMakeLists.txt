cmake_minimum_required(VERSION 3.1..3.29)

project(
    nukedopn2
    LANGUAGES CXX
)

add_library(
    nukedopn2 SHARED
    ym3438.c ym3438.h
)

target_include_directories(nukedopn2 PUBLIC .)