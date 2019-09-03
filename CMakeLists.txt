cmake_minimum_required(VERSION 3.13)
project(oneko C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(oneko
        oneko.c
        oneko.h
        patchlevel.h)

target_link_libraries(oneko X11 m Xext)
