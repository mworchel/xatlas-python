add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/pybind11)

add_library(xatlas-cpp STATIC ${CMAKE_CURRENT_LIST_DIR}/xatlas/source/xatlas/xatlas.h
                              ${CMAKE_CURRENT_LIST_DIR}/xatlas/source/xatlas/xatlas.cpp)

target_include_directories(xatlas-cpp PUBLIC ${CMAKE_CURRENT_LIST_DIR}/xatlas/source/xatlas)