cmake_minimum_required(VERSION 3.1..3.29)

project(
    vgmchips
    LANGUAGES CXX
)

add_subdirectory(psg)
add_subdirectory(opll)
add_subdirectory(opl)
add_subdirectory(ay8910)
add_subdirectory(scc)
add_subdirectory(opn2)
add_subdirectory(rf5c)
