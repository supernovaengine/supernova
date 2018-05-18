cmake_minimum_required(VERSION 3.0)

add_definitions("-DWITH_SDL_STATIC") # For SoLoud

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include ${PROJECT_SOURCE_DIR}/web_macros.h")
set(COMPILE_ZLIB OFF)
set(CMAKE_EXECUTABLE_SUFFIX ".html")

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../engine/core")

add_subdirectory (../../engine ${PROJECT_BINARY_DIR}/engine)
add_subdirectory (../../project ${PROJECT_BINARY_DIR}/project)

add_executable(
    supernova-emscripten

    main.cpp
    SupernovaWeb.cpp
)

set_target_properties(
    supernova-emscripten
    PROPERTIES LINK_FLAGS
    "-g4 -s ALLOW_MEMORY_GROWTH=1 --embed-file ../../../project/assets@/ -o supernova.html -s EXPORTED_FUNCTIONS=\"['_getScreenWidth','_getScreenHeight','_updateScreenSize','_main']\""
)

target_link_libraries(
    supernova-emscripten

    supernova
    supernova-project
)
