cmake_minimum_required(VERSION 3.14)
project(stl_clone)

set(CMAKE_CXX_STANDARD 14)

add_executable(stl_clone  main.c list.h list.c tree.h tree.c)