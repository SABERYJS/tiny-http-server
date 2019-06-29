cmake_minimum_required(VERSION 3.13)
project(rax)
aux_source_directory(. RAX_SOURCE_DIR)
add_library(rax ${RAX_SOURCE_DIR})