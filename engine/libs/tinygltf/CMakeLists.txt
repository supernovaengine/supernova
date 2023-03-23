include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../stb")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(

    tinygltf

    STATIC

    tiny_gltf.cc

)

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten" OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(
        tinygltf 

        PUBLIC 

        "-pthread"
    )
endif()

target_link_libraries(
    tinygltf
    stb
)

set_target_properties(
    tinygltf

    PROPERTIES

    CXX_STANDARD 11
)