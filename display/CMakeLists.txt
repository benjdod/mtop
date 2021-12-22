cmake_minimum_required(VERSION 3.10)

project(display)

add_library(display STATIC 
    drawbuffer.c
    dstring.c 
    dcolor.c
    screen.c 
    tty.c
)

target_include_directories(display 
    PUBLIC ".."
    PUBLIC "."
)