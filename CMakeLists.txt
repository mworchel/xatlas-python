cmake_minimum_required(VERSION 3.15...3.27)

project(xatlas-python LANGUAGES CXX
                      VERSION ${SKBUILD_PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Process external dependencies
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/extern)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src)