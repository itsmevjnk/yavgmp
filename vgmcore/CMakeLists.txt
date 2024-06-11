cmake_minimum_required(VERSION 3.1..3.29)

project(
    vgmcore
    LANGUAGES CXX
)

add_library(
    vgmcore SHARED
    src/vgm_parser.cpp src/vgm_header.cpp src/gd3.cpp src/vgm_data.cpp src/vgm_chip.cpp src/vgm_cmds.cpp src/vgm_chip_rateconv.cpp
    include/vgm_parser.h include/vgm_header.h include/gd3.h include/vgm_chip.h include/vgm_chip_rateconv.h
)

target_include_directories(vgmcore PUBLIC include)

add_subdirectory(zstr)
target_link_libraries(vgmcore PRIVATE zstr::zstr)

add_subdirectory(../vgmchips ../vgmchips)
target_link_libraries(vgmcore PUBLIC psg_native)
target_link_libraries(vgmcore PUBLIC psg_wrapper)
target_link_libraries(vgmcore PUBLIC opll_wrapper)