set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if( SUPERNOVA_GLES2 )
    file(GLOB SUPERNOVA_GLES2_SRCS "gles2/*.cpp")
endif()

add_library(
    supernova-renders

    STATIC

    ${SUPERNOVA_GLES2_SRCS}
)