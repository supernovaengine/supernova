cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include ${PROJECT_SOURCE_DIR}/web_macros.h")

set(COMPILE_ZLIB On)

add_subdirectory (../.. ${PROJECT_BINARY_DIR}/supernova)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../engine/core")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../engine/libs/lua")

add_executable(
    supernova-project
    main.cpp
    ../../project/main.cpp
)

set_target_properties(
    supernova-project
    PROPERTIES LINK_FLAGS
    "-s USE_GLFW=3 --embed-file ../../../assets@/ -o supernova-project.html"
)

target_link_libraries(
    supernova-project
    supernova
)
