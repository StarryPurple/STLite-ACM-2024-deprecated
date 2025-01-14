cmake_minimum_required(VERSION 3.28)
project(STLite-2024-deprecated)

set(CMAKE_CXX_STANDARD 14)

include_directories(vector priority_queue map)

add_executable(STLite_test
        main.cpp)
