cmake_minimum_required(VERSION 3.17)
project(sharp_demo)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(sharp_demo
        main.cpp
        sharpProtoc.cpp
        sharpProtoc.h
        sm4.c)
