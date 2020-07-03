cmake_minimum_required(VERSION 3.6)

if(NOT DEFINED APP_NAME)
   set(APP_NAME supernova-project)
endif()

project(${APP_NAME})

if(NOT DEFINED SUPERNOVA_ROOT)
    set(SUPERNOVA_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
endif()
if (NOT EXISTS "${SUPERNOVA_ROOT}")
    message(FATAL_ERROR "Can't find Supernova root directory: ${SUPERNOVA_ROOT}")
endif()

if(NOT DEFINED PROJECT_ROOT)
    set(PROJECT_ROOT ${SUPERNOVA_ROOT}/project)
endif()
if (NOT EXISTS "${PROJECT_ROOT}")
    message(FATAL_ERROR "Can't find project root directory: ${PROJECT_ROOT}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(COMPILE_ZLIB OFF)
set(IS_ARM OFF)
set(SUPERNOVA_GLES2 ON)

set(PLATFORM_ROOT)
set(PLATFORM_SOURCE)
set(PLATFORM_LIBS)
set(PLATFORM_RESOURCES)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    add_definitions("-DSUPERNOVA_WEB")
    add_definitions("-DWITH_SDL_STATIC") # For SoLoud

    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    set(PLATFORM_ROOT ${SUPERNOVA_ROOT}/platform/emscripten)

    list(APPEND PLATFORM_SOURCE
        ${PLATFORM_ROOT}/SupernovaWeb.cpp
    )

    list(APPEND PLATFORM_LIBS
        idbfs.js
    )
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
    set(APP_NAME supernova-android) # Android uses a fixed library name

    add_definitions("-DSUPERNOVA_ANDROID")
    add_definitions("-D\"lua_getlocaledecpoint()='.'\"")
    add_definitions("-DLUA_ANSI")
    add_definitions("-DLUA_USE_C89")
    add_definitions("-DWITH_MINIAUDIO") # For SoLoud

    if( ANDROID_ABI MATCHES "^arm(eabi)?(-v7a)?(64-v8a)?$" )
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon")
        set(IS_ARM ON)
    endif()

    set(PLATFORM_ROOT ${SUPERNOVA_ROOT}/platform/android)

    list(APPEND PLATFORM_SOURCE
        ${PLATFORM_ROOT}/AndroidJNI.cpp
        ${PLATFORM_ROOT}/SupernovaAndroid.cpp
    )

    list(APPEND PLATFORM_LIBS
        log android EGL GLESv2 OpenSLES
    )
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS")
    add_definitions("-DSUPERNOVA_IOS")
    add_definitions("-DWITH_MINIAUDIO") # For SoLoud

    set(PLATFORM_ROOT ${SUPERNOVA_ROOT}/platform/ios)

    include_directories (/System/Library/Frameworks)

    list(APPEND PLATFORM_SOURCE
        ${PLATFORM_ROOT}/main.m 
        ${PLATFORM_ROOT}/AppDelegate.h 
        ${PLATFORM_ROOT}/AppDelegate.m 
        ${PLATFORM_ROOT}/EAGLView.h
        ${PLATFORM_ROOT}/EAGLView.mm
        ${PLATFORM_ROOT}/GameViewController.h
        ${PLATFORM_ROOT}/GameViewController.mm
        ${PLATFORM_ROOT}/SupernovaIOS.h
        ${PLATFORM_ROOT}/SupernovaIOS.mm
        ${PLATFORM_ROOT}/IOSMacros.pch
    )

    list(APPEND PLATFORM_RESOURCES
        ${PLATFORM_ROOT}/Base.lproj/LaunchScreen.storyboard
        ${PLATFORM_ROOT}/Base.lproj/Main.storyboard
    )

    find_library(UIKIT_LIBRARY UIKit)
    find_library(OPENGLES_LIBRARY OpenGLES)
    find_library(AV_FOUNDATION_LIBRARY AVFoundation)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
    find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)

    list(APPEND PLATFORM_LIBS
        ${UIKIT_LIBRARY} ${OPENGLES_LIBRARY} ${AV_FOUNDATION_LIBRARY} ${QUARTZCORE_LIBRARY} ${AUDIOTOOLBOX_LIBRARY}
    )

endif()


include_directories ("${PLATFORM_ROOT}")
include_directories ("${SUPERNOVA_ROOT}/engine/core")
include_directories ("${SUPERNOVA_ROOT}/engine/core/action")
include_directories ("${SUPERNOVA_ROOT}/engine/core/action/keyframe")
include_directories ("${SUPERNOVA_ROOT}/engine/core/action/particleinit")
include_directories ("${SUPERNOVA_ROOT}/engine/core/action/particlemod")
include_directories ("${SUPERNOVA_ROOT}/engine/core/audio")
include_directories ("${SUPERNOVA_ROOT}/engine/core/buffer")
include_directories ("${SUPERNOVA_ROOT}/engine/core/image")
include_directories ("${SUPERNOVA_ROOT}/engine/core/io")
include_directories ("${SUPERNOVA_ROOT}/engine/core/math")
include_directories ("${SUPERNOVA_ROOT}/engine/core/physics")
include_directories ("${SUPERNOVA_ROOT}/engine/core/script")
include_directories ("${SUPERNOVA_ROOT}/engine/core/system")
include_directories ("${SUPERNOVA_ROOT}/engine/core/render")
include_directories ("${SUPERNOVA_ROOT}/engine/core/ui")
include_directories ("${SUPERNOVA_ROOT}/engine/core/util")

add_subdirectory (${SUPERNOVA_ROOT}/engine)

file(GLOB_RECURSE PROJECT_SOURCE ${PROJECT_ROOT}/*.cpp)

set(all_code_files
    ${PROJECT_SOURCE}

    ${PLATFORM_SOURCE}
    ${PLATFORM_RESOURCES}
)

if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL Android)
    add_executable(
        ${APP_NAME}
        ${all_code_files}
    )
else()
    add_library(
        ${APP_NAME}
        SHARED
        ${all_code_files}
    )
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    set_target_properties(
        ${APP_NAME}

        PROPERTIES 
        
        LINK_FLAGS
            "-g4 -s ALLOW_MEMORY_GROWTH=1 \
            --preload-file ${PROJECT_ROOT}/assets@/ \
            --preload-file ${PROJECT_ROOT}/lua@/ \
            -s EXPORTED_FUNCTIONS=\"['_getScreenWidth','_getScreenHeight','_changeCanvasSize','_main']\""
    )
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS")
    set(APP_BUNDLE_IDENTIFIER "org.supernovaengine.supernova") 
    
    set(PRODUCT_NAME ${APP_NAME})
    set(EXECUTABLE_NAME ${APP_NAME})
    set(MACOSX_BUNDLE_EXECUTABLE_NAME ${APP_NAME})
    set(MACOSX_BUNDLE_INFO_STRING ${APP_BUNDLE_IDENTIFIER})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_BUNDLE_IDENTIFIER})
    set(MACOSX_BUNDLE_BUNDLE_NAME ${APP_BUNDLE_IDENTIFIER})
    set(MACOSX_BUNDLE_ICON_FILE "")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "1.0")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
    set(MACOSX_BUNDLE_COPYRIGHT "(c) 2020 Eduardo Doria")
    
    set_target_properties(
        ${APP_NAME}
        
        PROPERTIES
    
        XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${PLATFORM_ROOT}/IOSMacros.pch"
        MACOSX_BUNDLE_INFO_PLIST ${PLATFORM_ROOT}/plist.in
        RESOURCE "${PLATFORM_RESOURCES}"
    )
endif()

target_link_libraries(
    ${APP_NAME}
 
    supernova
    ${PLATFORM_LIBS}
 )
