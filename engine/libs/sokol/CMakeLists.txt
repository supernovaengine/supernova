add_library(

    sokol

    STATIC

    sokol.cpp

)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(
        sokol

        GL dl m pthread X11 Xi Xcursor

        # For sokol_audio.h
        #asound
    )

    find_package(Threads REQUIRED)
    target_compile_options(sokol PUBLIC "-pthread")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set_source_files_properties(sokol.cpp PROPERTIES COMPILE_FLAGS "-x objective-c++")

    set(CMAKE_XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fobjc-arc")

    include_directories (/usr/local/include)
    link_directories(/usr/local/lib)

    target_link_libraries(
        sokol

        "-framework Cocoa"

        # For OpenGL
        #"-framework OpenGL"

        # For Metal
        "-framework Metal -framework MetalKit -framework Quartz"

        # For sokol_audio.h
        #"-framework AudioToolbox"
    )
endif()