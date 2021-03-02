cmake_minimum_required(VERSION 3.16)
project(voxeliser)

set(CMAKE_CXX_STANDARD 14)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bag_bk.obj ${CMAKE_CURRENT_BINARY_DIR}/bag_bk.obj COPYONLY)
add_executable(voxeliser main.cpp)