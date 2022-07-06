add_library(

    sokol

    STATIC

    sokol.cpp

)

set(SOKOL_LINK_LIBRARIES)
set(SOKOL_PROPERTIES)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        list(APPEND SOKOL_LINK_LIBRARIES
            user32 shell32 dxgi ole32 gdi32
        )
        if(GRAPHIC_BACKEND STREQUAL "d3d11")
            list(APPEND SOKOL_LINK_LIBRARIES
                d3d11
            )
        endif()
    endif()
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND SOKOL_LINK_LIBRARIES
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

    list(APPEND SOKOL_PROPERTIES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
    )

    include_directories (/usr/local/include)
    link_directories(/usr/local/lib)

    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        list(APPEND SOKOL_LINK_LIBRARIES "-framework Cocoa")
        if(GRAPHIC_BACKEND STREQUAL "glcore33")
            list(APPEND SOKOL_LINK_LIBRARIES "-framework OpenGL")
        elseif(GRAPHIC_BACKEND STREQUAL "metal")
            list(APPEND SOKOL_LINK_LIBRARIES "-framework Metal -framework MetalKit -framework Quartz")
        endif()
    elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        list(APPEND SOKOL_LINK_LIBRARIES "-framework UIKit")
        if(GRAPHIC_BACKEND STREQUAL "metal")
            list(APPEND SOKOL_LINK_LIBRARIES "-framework Metal -framework MetalKit")
        endif()
    endif()

    # For sokol_audio.h
    #"-framework AudioToolbox"
endif()

set_target_properties(
    sokol
    PROPERTIES

    CXX_STANDARD 17
    ${SOKOL_PROPERTIES}
)

target_link_libraries(
    sokol

    ${SOKOL_LINK_LIBRARIES}
)