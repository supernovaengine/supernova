cmake_minimum_required(VERSION 3.6)

project(Supernova)

add_definitions("-DWITH_SDL_STATIC") # For SoLoud

set(PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include ${PLATFORM_DIR}/WebMacros.h")
set(COMPILE_ZLIB OFF)
set(CMAKE_EXECUTABLE_SUFFIX ".html")
set(SUPERNOVA_GLES2 ON)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../engine/core")

add_subdirectory (../../engine ${PROJECT_BINARY_DIR}/engine)
add_subdirectory (../../project ${PROJECT_BINARY_DIR}/project)

add_executable(
    supernova-emscripten

    SupernovaWeb.cpp
)

set_target_properties(
    supernova-emscripten
    PROPERTIES LINK_FLAGS
    "-g4 -s ALLOW_MEMORY_GROWTH=1 --embed-file ../../../project/assets@/ -o supernova.html -s EXPORTED_FUNCTIONS=\"['_getScreenWidth','_getScreenHeight','_changeCanvasSize','_main']\""
)

target_link_libraries(
    supernova-emscripten

    supernova
    supernova-project
)