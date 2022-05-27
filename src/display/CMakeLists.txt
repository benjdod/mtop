cmake_minimum_required(VERSION 3.10)

#project(display)

add_library(display OBJECT 
    drawbuffer.c
    dcolor.c
    screen.c 
    tty.c
)

target_include_directories(display 
    PUBLIC ".."
    PUBLIC "."
)


#SET(GCC_COMPILE_FLAGS "-g -O2 -Wall")
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COMPILE_FLAGS}")

#add_compile_options(-g)
