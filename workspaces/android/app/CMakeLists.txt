 cmake_minimum_required(VERSION 3.0)

 add_definitions("-D\"lua_getlocaledecpoint()='.'\"")
 add_definitions("-DLUA_ANSI")
 add_definitions("-DLUA_USE_C89")
 add_definitions("-DWITH_OPENSLES") # For SoLoud

 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include ${PROJECT_SOURCE_DIR}/src/main/cpp/android_macros.h")

 set(COMPILE_ZLIB OFF)

 set(IS_ARM OFF)
 if( ANDROID_ABI MATCHES "^arm(eabi)?(-v7a)?(64-v8a)?$" )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon")
    set(IS_ARM ON)
 endif()

 include_directories(${ANDROID_NDK}/sources/android/cpufeatures)

 include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp")

 add_subdirectory (../../.. ${PROJECT_BINARY_DIR}/supernova)

 include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core")
 include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/action")
 include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/action/particleinit")
 include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/action/particlemod")
 include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/audio")
 include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/file")
 include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/image")
 include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/math")
 include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/physics")
 include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/platform")
 include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/render")
 include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../../../engine/core/util")

 add_library(
     supernova

     SHARED

     src/main/cpp/supernova_android_jni.cpp
     src/main/cpp/SupernovaAndroid.cpp
 )

  target_link_libraries(
       supernova

       supernova-core
       supernova-project

       log android EGL GLESv2 OpenSLES
  )
