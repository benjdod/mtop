cmake_minimum_required(VERSION 3.10)

project(process)

add_library(process STATIC 
    drawbuffer.c
    dstring.c 
    dcolor.c
    screen.c 
    tty.c
)

target_include_directories(process PUBLIC 
    ".."
    "."
)