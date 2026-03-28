# Required by cmake, some kind of minimum version
cmake_minimum_required(VERSION 3.10)
project(PROJ01_1 C)

# Set the C standard to latest C23
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

# Include Warnings and debugging flags
add_compile_options(-Wall -g)

add_library(libciblk SHARED lib/libciblk.c)
add_executable(simblock main.c)
target_link_libraries(simblock libciblk)