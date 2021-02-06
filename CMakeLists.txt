cmake_minimum_required(VERSION 3.12)

project(xatlas-python LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Process external dependencies
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/extern)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src)