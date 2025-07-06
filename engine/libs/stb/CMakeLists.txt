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

find_package(Threads REQUIRED)
target_link_libraries(stb PRIVATE Threads::Threads)

if(EMSCRIPTEN AND EMSCRIPTEN_THREAD_SUPPORT)
	target_compile_options(stb PRIVATE -pthread)
endif()