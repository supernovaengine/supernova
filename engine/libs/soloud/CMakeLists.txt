# SoLoud - https://github.com/jarikomppa/soloud/

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../stb")

add_library(

    soloud

    STATIC

    src/core/soloud_audiosource.cpp
    src/core/soloud_bus.cpp
    src/core/soloud_core_3d.cpp
    src/core/soloud_core_basicops.cpp
    src/core/soloud_core_faderops.cpp
    src/core/soloud_core_filterops.cpp
    src/core/soloud_core_getters.cpp
    src/core/soloud_core_setters.cpp
    src/core/soloud_core_voicegroup.cpp
    src/core/soloud_core_voiceops.cpp
    src/core/soloud_fader.cpp
    src/core/soloud_fft_lut.cpp
    src/core/soloud_fft.cpp
    src/core/soloud_file.cpp
    src/core/soloud_filter.cpp
    src/core/soloud_misc.cpp
    src/core/soloud_queue.cpp
    src/core/soloud_thread.cpp
    src/core/soloud.cpp

    src/audiosource/wav/dr_impl.cpp
    src/audiosource/wav/soloud_wav.cpp

    #src/backend/opensles/soloud_opensles.cpp
    #src/backend/sdl_static/soloud_sdl_static.cpp
    src/backend/miniaudio/soloud_miniaudio.cpp

    src/filter/soloud_bassboostfilter.cpp
    src/filter/soloud_biquadresonantfilter.cpp
    src/filter/soloud_dcremovalfilter.cpp
    src/filter/soloud_duckfilter.cpp
    src/filter/soloud_echofilter.cpp
    src/filter/soloud_eqfilter.cpp
    src/filter/soloud_fftfilter.cpp
    src/filter/soloud_flangerfilter.cpp
    src/filter/soloud_freeverbfilter.cpp
    src/filter/soloud_lofifilter.cpp
    src/filter/soloud_robotizefilter.cpp
    src/filter/soloud_waveshaperfilter.cpp
)

if(APPLE)
    #set_source_files_properties(src/backend/miniaudio/soloud_miniaudio.cpp PROPERTIES LANGUAGE OBJCXX)
    set_source_files_properties(src/backend/miniaudio/soloud_miniaudio.cpp PROPERTIES XCODE_EXPLICIT_FILE_TYPE "sourcecode.cpp.objcpp")
endif()

target_link_libraries(
        soloud
        stb
)
