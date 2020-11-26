# cmake_minimum_required(VERSION <specify CMake version here>)
project(sm4_demo C CXX)

set(CMAKE_CXX_STANDARD 11)

add_executable(sm4_demo
        sm4.c
        sm4test.cc
        sm4_ecb.h
        )
