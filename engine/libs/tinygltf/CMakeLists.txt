include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../stb")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(

    tinygltf

    STATIC

    tiny_gltf.cc

)

target_link_libraries(
    tinygltf
    stb
)
