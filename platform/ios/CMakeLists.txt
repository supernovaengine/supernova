cmake_minimum_required(VERSION 3.6)

project(Supernova)

add_definitions("-DWITH_MINIAUDIO") # For SoLoud

set(PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(/System/Library/Frameworks)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include ${PLATFORM_DIR}/IOSMacros.pch")
set(SUPERNOVA_GLES2 ON)

 if(NOT DEFINED SUPERNOVA_ROOT)
    set(SUPERNOVA_ROOT ../..)
 endif()

 if(NOT DEFINED PROJECT_SOURCE)
    set(PROJECT_SOURCE ${SUPERNOVA_ROOT}/project)
 endif()

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/${SUPERNOVA_ROOT}/engine/core")

add_subdirectory (${SUPERNOVA_ROOT}/engine ${PROJECT_BINARY_DIR}/engine)
add_subdirectory (${PROJECT_SOURCE} ${PROJECT_BINARY_DIR}/project)

set(RESOURCES
    Base.lproj/LaunchScreen.storyboard
    Base.lproj/Main.storyboard
)

add_executable(
    supernova-ios

    main.m

    AppDelegate.h 
    AppDelegate.m 
    EAGLView.h
    EAGLView.mm
    GameViewController.h
    GameViewController.mm
    SupernovaIOS.h
    SupernovaIOS.mm

    ${RESOURCES}

    IOSMacros.pch
)

set(APP_NAME "Supernova") 
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
    supernova-ios
    
    PROPERTIES

    XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/IOSMacros.pch"
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/plist.in
    RESOURCE "${RESOURCES}"
)


find_library(UIKIT_LIBRARY UIKit)
find_library(OPENGLES_LIBRARY OpenGLES)
find_library(AV_FOUNDATION_LIBRARY AVFoundation)
find_library(QUARTZCORE_LIBRARY QuartzCore)
find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)

target_link_libraries(
    supernova-ios

    supernova
    supernova-project

    ${UIKIT_LIBRARY} ${OPENGLES_LIBRARY} ${AV_FOUNDATION_LIBRARY} ${QUARTZCORE_LIBRARY} ${AUDIOTOOLBOX_LIBRARY}
)