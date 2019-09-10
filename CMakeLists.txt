cmake_minimum_required(VERSION 2.8)
add_definitions("-std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project(MP1)

add_executable(
        Memtest
        my_allocator.cpp
        free_list.cpp
        memtest.cpp
)
