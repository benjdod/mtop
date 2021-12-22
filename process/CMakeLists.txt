cmake_minimum_required(VERSION 3.10)

project(process)

add_library(process STATIC 
    proc.c
    procbst.c 
    procdraw.c
)

target_include_directories(process
    PRIVATE ".."
    PUBLIC "."
)