add_library(
    tinyxml2

    STATIC

    tinyxml2.cpp
)

find_package(Threads REQUIRED)
target_link_libraries(tinyxml2 PRIVATE Threads::Threads)

if(EMSCRIPTEN AND EMSCRIPTEN_THREAD_SUPPORT)
	target_compile_options(tinyxml2 PRIVATE -pthread)
endif()
