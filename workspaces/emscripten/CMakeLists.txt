cmake_minimum_required(VERSION 3.0)

add_definitions("-DWITH_SDL_STATIC") # For SoLoud

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include ${PROJECT_SOURCE_DIR}/web_macros.h")

set(COMPILE_ZLIB OFF)

add_subdirectory (../.. ${PROJECT_BINARY_DIR}/supernova)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../engine/core")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/action")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/audio")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/data")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/file")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/image")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/math")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/platform")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/render")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/text")

file(GLOB_RECURSE SUPERNOVA_PROJECT_SRCS ../../project/*.cpp)

add_library(
    supernova
    main.cpp
)

target_link_libraries(
    supernova
    supernova-core
)

add_executable(
    supernova-project
    ${SUPERNOVA_PROJECT_SRCS}
)

set_target_properties(
    supernova-project
    PROPERTIES LINK_FLAGS
    "-g4 -s ALLOW_MEMORY_GROWTH=1 --embed-file ../../../project/assets@/ -o supernova-project.html -s EXPORTED_FUNCTIONS=\"['_getScreenWidth','_getScreenHeight','_updateScreenSize','_main']\""
)

target_link_libraries(
    supernova-project
    supernova
)
