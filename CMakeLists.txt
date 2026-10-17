cmake_minimum_required(VERSION 3.13)
project(assoc_structs CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(assoc_structs src/main.cpp)
target_include_directories(assoc_structs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)