cmake_minimum_required(VERSION 3.15)
project(wsdemo)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(wsdemo
        base64.cpp
        base64.h
        main.cpp
        network_interface.cpp
        network_interface.h
        sha1.cpp
        sha1.h
        websocket_handler.cpp
        websocket_handler.h
        ws_protocol.cpp
        ws_protocol.h)
