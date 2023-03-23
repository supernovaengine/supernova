add_library(
    tinyobjloader

    STATIC

    tiny_obj_loader.cc
)

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten" OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(
        tinyobjloader 

        PUBLIC 

        "-pthread"
    )
endif()
