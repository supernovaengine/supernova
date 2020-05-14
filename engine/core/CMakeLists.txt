set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB_RECURSE SUPERNOVA_SRCS *.cpp)

add_library(
    supernova

    STATIC

    ${SUPERNOVA_SRCS}
)

target_link_libraries(
    supernova

    supernova-renders
    lua luaintf tinyobjloader soloud stb box2d tinygltf
)