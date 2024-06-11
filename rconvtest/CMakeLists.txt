cmake_minimum_required(VERSION 3.1..3.29)

project(
    rconvtest
    LANGUAGES CXX
)

add_executable(
    rconvtest
    main.cpp
)

target_link_libraries(rconvtest PUBLIC vgmcore)