 cmake_minimum_required(VERSION 3.6)

 project (Supernova)

 add_definitions("-D\"lua_getlocaledecpoint()='.'\"")
 add_definitions("-DLUA_ANSI")
 add_definitions("-DLUA_USE_C89")
 add_definitions("-DWITH_MINIAUDIO") # For SoLoud

 set(PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include ${PLATFORM_DIR}/AndroidMacros.h")
 set(COMPILE_ZLIB OFF)
 set(IS_ARM OFF)
 set(SUPERNOVA_GLES2 ON)

 if( ANDROID_ABI MATCHES "^arm(eabi)?(-v7a)?(64-v8a)?$" )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon")
    set(IS_ARM ON)
 endif()

 if(NOT DEFINED SUPERNOVA_ROOT)
    set(SUPERNOVA_ROOT ../..)
 endif()

 if(NOT DEFINED PROJECT_SOURCE)
    set(PROJECT_SOURCE project)
 endif()

 include_directories ("${ANDROID_NDK}/sources/android/cpufeatures")
 include_directories ("${PLATFORM_DIR}")
 include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/${SUPERNOVA_ROOT}/engine/core")

 add_subdirectory (${SUPERNOVA_ROOT}/engine ${PROJECT_BINARY_DIR}/engine)
 add_subdirectory (${SUPERNOVA_ROOT}/${PROJECT_SOURCE} ${PROJECT_BINARY_DIR}/project)

 add_library(
     supernova-android

     SHARED

     AndroidJNI.cpp
     SupernovaAndroid.cpp
 )

  target_link_libraries(
       supernova-android

       supernova
       supernova-project

       log android EGL GLESv2 OpenSLES
  )
