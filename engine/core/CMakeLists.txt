set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB_RECURSE SUPERNOVA_SRCS *.cpp)

add_library(
    supernova-core

    STATIC

    ${SUPERNOVA_SRCS}
)

target_link_libraries(
    supernova-core
    lua tinyobjloader mpg123 soloud stb box2d
)
