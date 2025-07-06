set (
    LUA_SOURCE

    lapi.c
    lauxlib.c
    lbaselib.c
    lcode.c
    lcorolib.c
    lctype.c
    ldblib.c
    ldebug.c
    ldo.c
    ldump.c
    lfunc.c
    lgc.c
    linit.c
    liolib.c
    llex.c
    lmathlib.c
    lmem.c
    loadlib.c
    lobject.c
    lopcodes.c
    loslib.c
    lparser.c
    lstate.c
    lstring.c
    lstrlib.c
    ltable.c
    ltablib.c
    ltm.c
    lundump.c
    lutf8lib.c
    lvm.c
    lzio.c
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions("-DLUA_USE_WINDOWS")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_definitions("-DLUA_USE_MACOSX")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions("-DLUA_USE_LINUX")
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    add_definitions("-DLUA_USE_IOS")
endif()

add_library(
    lua

    STATIC

    ${LUA_SOURCE}
)

find_package(Threads REQUIRED)
target_link_libraries(lua PRIVATE Threads::Threads)

if(EMSCRIPTEN AND EMSCRIPTEN_THREAD_SUPPORT)
	target_compile_options(lua PRIVATE -pthread)
endif()