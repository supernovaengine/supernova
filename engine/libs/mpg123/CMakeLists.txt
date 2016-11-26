# mpg123 v 1.23.8

add_definitions("-w")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_GCC_DESTRUCTOR=1 -DOPT_GENERIC -DREAL_IS_FLOAT")

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/src/compat")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/src/libmpg123")

set (
    mpg_sources

    ${mpg_sources}

#   src/libmpg123/dct64_altivec.c
#   src/libmpg123/dct64_i386.c
#   src/libmpg123/dct64_i486.c
    src/libmpg123/dct64.c
    src/libmpg123/dither.c
    src/libmpg123/equalizer.c
    src/libmpg123/feature.c
    src/libmpg123/format.c
    src/libmpg123/frame.c
    src/libmpg123/icy.c
    src/libmpg123/icy2utf8.c
    src/libmpg123/id3.c
    src/libmpg123/index.c
    src/libmpg123/layer1.c
    src/libmpg123/layer2.c
    src/libmpg123/layer3.c
    src/libmpg123/lfs_alias.c
#   src/libmpg123/lfs_wrap.c
    src/libmpg123/libmpg123.c
    src/libmpg123/ntom.c
    src/libmpg123/parse.c
    src/libmpg123/optimize.c
    src/libmpg123/readers.c
    src/libmpg123/stringbuf.c
    src/libmpg123/synth_8bit.c
#   src/libmpg123/synth_altivec.c
#   src/libmpg123/synth_i486.c
#   src/libmpg123/synth_real.c
#   src/libmpg123/synth_s32.c
    src/libmpg123/synth.c
    src/libmpg123/tabinit.c
#   src/libmpg123/testcpu.c

    src/compat/compat.c
    src/compat/compat_str.c
)


if (IS_ARM)
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DOPT_ARM")

#    set (
#        mpg_sources
#
#        ${mpg_sources}
#
#        src/libmpg123/getcpuflags_arm.c
#    )
endif (IS_ARM)

add_library(
    mpg123

    STATIC

    ${mpg_sources}
)
