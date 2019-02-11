add_library(

    tinygltf

    STATIC

    tiny_gltf.cc

)

target_link_libraries(
    tinygltf
    stb
)
