cmake_minimum_required(VERSION 3.27)
project(untitled1)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(
        TYPES="${TYPES}"
        SIZES="${SIZES}"
)



configure_file(macros.h.in macros.h)

add_executable(fluid fluid.cpp Fixed.h)

