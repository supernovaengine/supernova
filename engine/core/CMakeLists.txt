set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB_RECURSE SUPERNOVA_SRCS *.cpp)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/platform")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/platform/generic")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/platform/ios")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/platform/android")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/platform/web")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/file")

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
