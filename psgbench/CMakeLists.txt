cmake_minimum_required(VERSION 3.1..3.29)

project(
    psgbench
    LANGUAGES CXX
)

add_executable(
    psgbench
    main.cpp
)

target_link_libraries(psgbench PUBLIC vgmcore)