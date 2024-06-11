cmake_minimum_required(VERSION 3.1..3.29)

project(
    vgmchips_ay8910
    LANGUAGES CXX
)

# uncomment to use emu2149's built-in rate converter
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEMU2149_USE_BUILTIN_RATECONV")

add_library(
    emu2149_wrapper STATIC
    emu2149_wrapper.cpp emu2149_wrapper.h
)

target_include_directories(emu2149_wrapper PUBLIC .)
target_include_directories(emu2149_wrapper PUBLIC ../../vgmcore/include)

add_subdirectory(emu2149)
target_link_libraries(emu2149_wrapper PRIVATE emu2149)