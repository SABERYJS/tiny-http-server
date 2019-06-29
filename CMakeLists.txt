cmake_minimum_required(VERSION 3.13)
project(qc)

set(CMAKE_CXX_STANDARD 14)
add_subdirectory(lib/rax/)
add_subdirectory(src/algorithm/)
include_directories(lib/rax)
include_directories(src/algorithm)
add_executable(qc src/main.c)
target_link_libraries(qc rax stl)

add_executable(test-rax test/test-rax.c)
add_executable(test-regex test/test-regex.c)
target_link_libraries(test-rax rax)
target_link_libraries(test-regex regex)

