add_library(
    tinyxml2

    STATIC

    tinyxml2.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten" OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(
        tinyxml2 

        PUBLIC 

        "-pthread"
    )
endif()
