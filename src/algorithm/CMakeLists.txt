cmake_minimum_required(VERSION 3.13)
project(stl)
aux_source_directory(. COMMON_ALGORITHM)
add_library(stl ${COMMON_ALGORITHM})