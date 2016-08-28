cmake_minimum_required(VERSION 3.0)

add_definitions("-D\"lua_getlocaledecpoint()='.'\"")
add_definitions("-DLUA_ANSI")
add_definitions("-DLUA_USE_C89")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include ${PROJECT_SOURCE_DIR}/src/main/cpp/android_macros.h")

set(COMPILE_ZLIB OFF)

add_subdirectory (../../.. ${PROJECT_BINARY_DIR}/supernova)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/libs/lua")

add_library(
    supernova-project

    SHARED
    
    src/main/cpp/supernova_android_jni.cpp
    src/main/cpp/android_fopen.c
    src/main/cpp/androidcallback.cpp
    ../../../project/main.cpp
)

target_link_libraries(
    supernova-project
    log android EGL GLESv2
    supernova
)
