add_library(
    tinyobjloader

    STATIC

    tiny_obj_loader.cc
)

find_package(Threads REQUIRED)
target_link_libraries(tinyobjloader PRIVATE Threads::Threads)

if(EMSCRIPTEN AND EMSCRIPTEN_THREAD_SUPPORT)
	target_compile_options(tinyobjloader PRIVATE -pthread)
endif()
