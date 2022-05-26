cmake_minimum_required(VERSION 3.10)

project(process)

add_library(process STATIC 
    proc.c
    draw.c
)

target_include_directories(process
    PRIVATE ".."
    PUBLIC "."
)


#SET(GCC_COMPILE_FLAGS "-g -O2 -Wall")
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COMPILE_FLAGS}")

#add_compile_options(-g)
