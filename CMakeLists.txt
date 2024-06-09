cmake_minimum_required(VERSION 3.1..3.29)

project(yavgmp)

set(CMAKE_CXX_FLAGS "-O3")

# compile libvgmcore first
add_subdirectory(vgmcore)

# then we can compile the tools that depend on libvgmcore
add_subdirectory(vgminfo)
add_subdirectory(vgmcdump)
add_subdirectory(psgbench)