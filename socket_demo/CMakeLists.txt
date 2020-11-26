cmake_minimum_required(VERSION 3.15)
project(socket_demo)

set(CMAKE_CXX_STANDARD 11)

add_executable(socket_server server.cpp)
add_executable(socket_client client.cpp)