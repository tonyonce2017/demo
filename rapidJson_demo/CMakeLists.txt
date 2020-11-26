cmake_minimum_required(VERSION 3.15)
project(rapidJson_demo)

set(CMAKE_CXX_STANDARD 11)

include_directories(
        ./rapidjson
)

add_executable(rapidJson_demo main.cpp)