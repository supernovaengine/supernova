file(GLOB_RECURSE SUPERNOVA_SRCS *.cpp)

add_library(
    supernova

    STATIC

    ${SUPERNOVA_SRCS}
)

target_link_libraries(
    supernova
    lua png tinyobjloader
)
