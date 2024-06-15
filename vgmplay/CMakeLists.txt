cmake_minimum_required(VERSION 3.1..3.29)

project(
    vgmplay
    LANGUAGES CXX
)

add_library(
    pa_wrapper STATIC
    pa-wrapper/pa.cpp pa-wrapper/pa.h
)

target_include_directories(pa_wrapper PUBLIC pa-wrapper)

find_package(PortAudio REQUIRED)
target_link_libraries(pa_wrapper PUBLIC portaudio)

add_executable(
    vgmplay
    main.cpp
)

target_link_libraries(vgmplay PUBLIC pthread)
target_link_libraries(vgmplay PUBLIC vgmcore)
target_link_libraries(vgmplay PUBLIC pa_wrapper)

