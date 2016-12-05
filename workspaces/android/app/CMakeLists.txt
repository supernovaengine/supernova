 cmake_minimum_required(VERSION 3.0)

 add_definitions("-D\"lua_getlocaledecpoint()='.'\"")
 add_definitions("-DLUA_ANSI")
 add_definitions("-DLUA_USE_C89")

 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include ${PROJECT_SOURCE_DIR}/src/main/cpp/android_macros.h")

 set(COMPILE_ZLIB OFF)
 set(COMPILE_OPENAL ON)

 set(IS_ARM OFF)
 if( ANDROID_ABI MATCHES "^arm(eabi)?(-v7a)?(64-v8a)?$" )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon")
    set(IS_ARM ON)
 endif()

 include_directories(${ANDROID_NDK}/sources/android/cpufeatures)

 add_subdirectory (../../.. ${PROJECT_BINARY_DIR}/supernova)

 include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core")
 include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/libs/lua")

 add_library(
     supernova

     SHARED

     src/main/cpp/supernova_android_jni.cpp
     src/main/cpp/android_fopen.cpp
     src/main/cpp/androidcallback.cpp
 )

  add_library(
      supernova-project
      ../../../project/main.cpp
  )

 target_link_libraries(
     supernova
     log android EGL GLESv2 OpenSLES

     supernova-core
     supernova-project
 )
