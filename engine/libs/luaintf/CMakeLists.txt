# lua-intf - https://github.com/SteveKChiu/lua-intf

add_definitions("-DLUAINTF_HEADERS_ONLY=0")

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../lua")

add_library(

    luaintf

    STATIC

    LuaIntf/src/CppBindClass.cpp
    LuaIntf/src/CppBindModule.cpp
    LuaIntf/src/CppFunction.cpp
    LuaIntf/src/CppObject.cpp
    LuaIntf/src/LuaCompat.cpp
    LuaIntf/src/LuaRef.cpp
    LuaIntf/src/LuaState.cpp
    #LuaIntf/src/QtLuaIntf.cpp
)

target_link_libraries(
        luaintf
        lua
)
