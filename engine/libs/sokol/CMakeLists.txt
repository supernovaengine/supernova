add_library(

    sokol

    STATIC

    sokol.cpp

)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_libraries(
            sokol

            kernel32 user32 shell32 dxgi d3d11 ole32 gdi32
        )
    endif()
endif()

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

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set_source_files_properties(sokol.cpp PROPERTIES COMPILE_FLAGS "-x objective-c++")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fobjc-arc")

    set_target_properties(
        sokol
        PROPERTIES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
    )

    include_directories (/usr/local/include)
    link_directories(/usr/local/lib)

    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        if(GRAPHIC_BACKEND STREQUAL "glcore33")
            set(SOKOL_LINK_LIBRARIES "-framework Cocoa -framework OpenGL")
        elseif(GRAPHIC_BACKEND STREQUAL "metal")
            set(SOKOL_LINK_LIBRARIES "-framework Cocoa -framework Metal -framework MetalKit -framework Quartz")
        endif()
    elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        if(GRAPHIC_BACKEND STREQUAL "metal")
            set(SOKOL_LINK_LIBRARIES "-framework UIKit -framework Metal -framework MetalKit")
        endif()
    endif()

    target_link_libraries(
        sokol

        ${SOKOL_LINK_LIBRARIES}

        # For sokol_audio.h
        #"-framework AudioToolbox"
    )
endif()