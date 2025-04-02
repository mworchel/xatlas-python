pybind11_add_module(xatlas module.cpp 
                           atlas.hpp atlas.cpp
                           options.hpp options.cpp
                           utils.hpp utils.cpp)

target_link_libraries(xatlas PRIVATE xatlas-cpp)

target_compile_definitions(xatlas PRIVATE VERSION_INFO=${PROJECT_VERSION})

# The install directory is the output (wheel) directory
install(TARGETS xatlas DESTINATION .)