cmake_minimum_required(VERSION 3.0)

add_definitions("-DWITH_SDL2_STATIC") # For SoLoud
add_definitions("-s USE_SDL=2")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include ${PROJECT_SOURCE_DIR}/web_macros.h")

set(COMPILE_ZLIB OFF)

add_subdirectory (../.. ${PROJECT_BINARY_DIR}/supernova)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../engine/core")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../engine/libs/lua")

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
    ../../project/main.cpp
)

set_target_properties(
    supernova-project
    PROPERTIES LINK_FLAGS
    "-g4 -s ALLOW_MEMORY_GROWTH=1 -s USE_GLFW=3 --embed-file ../../../assets@/ -o supernova-project.html -s EXPORTED_FUNCTIONS=\"['_getScreenWidth','_getScreenHeight','_updateScreenSize','_main']\""
)

target_link_libraries(
    supernova-project
    supernova
)
