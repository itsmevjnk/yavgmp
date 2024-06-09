cmake_minimum_required(VERSION 3.1..3.29)

project(
    vgmcdump
    LANGUAGES CXX
)

add_executable(
    vgmcdump
    main.cpp
)

target_link_libraries(vgmcdump PUBLIC vgmcore)