set (
    LUA_SOURCE

    lapi.c
    lauxlib.c
    lbaselib.c
    lbitlib.c
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

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions("-DLUA_USE_POSIX")
endif()

add_library(
    lua

    STATIC

    ${LUA_SOURCE}
)

# By default Lua-Intf expect the Lua library to build under C++
set_source_files_properties(
    ${LUA_SOURCE}
    PROPERTIES LANGUAGE CXX
)
