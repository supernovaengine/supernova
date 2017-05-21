set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB_RECURSE SUPERNOVA_SRCS *.cpp)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/platform")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/platform/generic")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/platform/ios")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/platform/android")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/platform/web")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/file")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/audio")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/text")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/image")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/render")

add_library(
    supernova-core

    STATIC

    ${SUPERNOVA_SRCS}
)

target_link_libraries(
    supernova-core
    lua tinyobjloader jpeg png tiff mpg123 soloud stb
)
