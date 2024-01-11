add_library(

    stb

    STATIC

    stb_vorbis.c
    stb_rect_pack.c
    stb_truetype.c
    stb_image.c
    stb_image_write.c
    stb_image_resize2.c

)

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten" OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(
        stb 

        PUBLIC 

        "-pthread"
    )
endif()