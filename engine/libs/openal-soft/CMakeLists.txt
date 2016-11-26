# OpenAL-Soft v 1.17.2

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DAL_BUILD_LIBRARY -DAL_ALEXT_PROTOTYPES -DOPENAL_SUBDIR_AL")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DAL_ALEXT_PROTOTYPES -DAL_BUILD_LIBRARY -D_GNU_SOURCE=1 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -Drestrict=__restrict")


include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/OpenAL32/Include")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/Alc")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/Alc/backends")


add_library(
    openal

    STATIC

    OpenAL32/alAuxEffectSlot.c
	OpenAL32/alBuffer.c
	OpenAL32/alEffect.c
	OpenAL32/alError.c
	OpenAL32/alExtension.c
	OpenAL32/alFilter.c
	OpenAL32/alListener.c
	OpenAL32/alSource.c
	OpenAL32/alState.c
	OpenAL32/alThunk.c
	Alc/ALc.c
	Alc/alcConfig.c
	Alc/alcRing.c
	Alc/ALu.c
	Alc/bs2b.c
	Alc/mixer.c
	Alc/panning.c

#    Alc/mixer_neon.c
#    Alc/mixer_inc.c
#    Alc/mixer_sse.c
#    Alc/mixer_sse2.c
#    Alc/mixer_sse41.c

    Alc/bsinc.c
    Alc/helpers.c
    Alc/hrtf.c
    Alc/mixer_c.c
    Alc/effects/autowah.c
    Alc/effects/chorus.c
    Alc/effects/compressor.c
    Alc/effects/dedicated.c
    Alc/effects/distortion.c
    Alc/effects/echo.c
    Alc/effects/equalizer.c
    Alc/effects/flanger.c
    Alc/effects/modulator.c
    Alc/effects/null.c
    Alc/effects/reverb.c
    Alc/backends/base.c
    Alc/backends/opensl.c
    Alc/backends/null.c
    Alc/backends/loopback.c
    Alc/backends/wave.c
    OpenAL32/sample_cvt.c
    common/atomic.c
    common/rwlock.c
    common/threads.c
    common/uintmap.c
)
