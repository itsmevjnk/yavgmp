cmake_minimum_required(VERSION 3.1..3.29)

project(
    vgminfo
    LANGUAGES CXX
)

add_executable(
    vgminfo
    main.cpp
)

target_link_libraries(vgminfo PUBLIC vgmcore)