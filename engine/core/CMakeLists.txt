set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB_RECURSE SUPERNOVA_SRCS *.cpp)

add_library(
    supernova-core

    STATIC

    ${SUPERNOVA_SRCS}
)

target_link_libraries(
    supernova-core
    lua tinyobjloader jpeg png tiff mpg123 freetype soloud stb
)

if (COMPILE_OPENAL)
    target_link_libraries(
        supernova-core
        openal
    )
endif (COMPILE_OPENAL)
