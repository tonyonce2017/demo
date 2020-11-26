cmake_minimum_required(VERSION 3.15)
project(pthread_demo)

set(CMAKE_CXX_STANDARD 11)

add_executable(pthread_demo main.cpp)

target_link_libraries(pthread_demo
        libpthread.so)