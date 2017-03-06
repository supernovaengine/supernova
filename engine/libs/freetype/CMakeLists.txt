# Freetype version 2.7.1
# Customized ftmodule.h - Removed some modules
# Customized ftoption.h - Added macro FT_CONFIG_OPTION_SYSTEM_ZLIB

add_definitions("-w")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFT2_BUILD_LIBRARY=1")

add_library(
    freetype

    STATIC

    src/autofit/autofit.c
    src/base/basepic.c
    src/base/ftapi.c
    src/base/ftbase.c
    src/base/ftbbox.c
    src/base/ftbitmap.c
    src/base/ftdbgmem.c
    src/base/ftdebug.c
    src/base/ftglyph.c
    src/base/ftinit.c
    src/base/ftpic.c
    src/base/ftstroke.c
    src/base/ftsynth.c
    src/base/ftsystem.c
    src/cff/cff.c
    src/gzip/ftgzip.c
    src/pshinter/pshinter.c
    src/psnames/psnames.c
    src/raster/raster.c
    src/sfnt/sfnt.c
    src/smooth/smooth.c
    src/truetype/truetype.c
)

target_link_libraries(
    freetype
    z
)
