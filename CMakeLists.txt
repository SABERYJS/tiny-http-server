cmake_minimum_required(VERSION 3.13)
project(stl_clone)

set(CMAKE_CXX_STANDARD 14)

add_executable(stl_clone  main.c list.h list.c tree.h tree.c array.c array.h test/array-test.c unit.c unit.h hash.c hash.h test/hash-test.h tool.c tool.h stack.c stack.h config_header.h test/stack-test.h queue.c queue.h test/queue-test.h)