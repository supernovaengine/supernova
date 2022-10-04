//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaBridge.h"

#include "io/Data.h"
#include "io/File.h"
#include "io/FileData.h"
#include "io/UserSettings.h"

using namespace Supernova;

void LuaBinding::registerIOClasses(lua_State *L){
#ifndef DISABLE_LUA_BINDINGS

    luabridge::getGlobalNamespace(L)
        .beginClass<FileData>("FileData")
        .addStaticFunction("newFile", +[](lua_State* L) -> FileData* { 
            if (lua_gettop(L) != 1 && lua_gettop(L) != 2) throw luaL_error(L, "incorrect argument number");
            if (lua_isstring(L, -2) && lua_isboolean(L, -1)) return FileData::newFile(lua_tostring(L, -2), lua_toboolean(L, -1));
            if (lua_isboolean(L, -1)) return FileData::newFile(lua_toboolean(L, -1));
            throw luaL_error(L, "incorrect argument type");
            })
        .addStaticFunction("getBaseDir", &FileData::getBaseDir)
        .addStaticFunction("getFilePathExtension", &FileData::getFilePathExtension)
        .addStaticFunction("getSystemPath", &FileData::getSystemPath)
        .addFunction("read8", &FileData::read8)
        .addFunction("read16", &FileData::read16)
        .addFunction("read32", &FileData::read32)
        .addFunction("eof", &FileData::eof)
        //.addFunction("read", &FileData::read) //TODO: read and write data in Lua
        //.addFunction("write", &FileData::write)
        .addFunction("length", &FileData::length)
        .addFunction("seek", &FileData::seek)
        .addFunction("pos", &FileData::pos)
        .addFunction("readString", &FileData::readString)
        .addFunction("writeString", &FileData::writeString)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<File, FileData>("File")
        .addConstructor (
            [](void* ptr, lua_State* L) { 
                if (lua_gettop(L) == 2)
                    return new (ptr) File();
                if (lua_gettop(L) == 4)
                    return new (ptr) File(luaL_checkstring(L, 2), lua_toboolean(L, 3)); 
                throw luaL_error(L, "This is not a valid constructor");
            }
        )
        .endClass();

    luabridge::getGlobalNamespace(L)
        .deriveClass<Data, FileData>("Data")
        .addConstructor (
            [](void* ptr, lua_State* L) { 
                if (lua_gettop(L) == 2)
                    return new (ptr) Data();
                if (lua_gettop(L) == 3)
                    return new (ptr) Data(luaL_checkstring(L, 2));
                //TODO: read and write data in Lua
                //if (lua_gettop(L) == 6)
                //    return new (ptr) Data(luaL_checkstring(L, 2), luaL_checkinteger(L, 3), lua_toboolean(L, 4), lua_toboolean(L, 5)); 
                throw luaL_error(L, "This is not a valid constructor");
            }
        )
        .addFunction("open", (unsigned int(Data::*)(const char *))&Data::open)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<UserSettings>("UserSettings")
        .addStaticFunction("getBoolForKey", (bool(*)(const char *, bool))&UserSettings::getBoolForKey)
        .addStaticFunction("getIntegerForKey", (int(*)(const char *, int))&UserSettings::getIntegerForKey)
        .addStaticFunction("getLongForKey", (long(*)(const char *, long))&UserSettings::getLongForKey)
        .addStaticFunction("getFloatForKey", (float(*)(const char *, float))&UserSettings::getFloatForKey)
        .addStaticFunction("getDoubleForKey", (double(*)(const char *, double))&UserSettings::getDoubleForKey)
        .addStaticFunction("getStringForKey", (std::string(*)(const char *, std::string))&UserSettings::getStringForKey)
        .addStaticFunction("getDataForKey", (Data(*)(const char *, const Data&))&UserSettings::getDataForKey)
        .addStaticFunction("setBoolForKey", &UserSettings::setBoolForKey)
        .addStaticFunction("setIntegerForKey", &UserSettings::setIntegerForKey)
        .addStaticFunction("setLongForKey", &UserSettings::setLongForKey)
        .addStaticFunction("setFloatForKey", &UserSettings::setFloatForKey)
        .addStaticFunction("setDoubleForKey", &UserSettings::setDoubleForKey)
        .addStaticFunction("setStringForKey", &UserSettings::setStringForKey)
        .addStaticFunction("setDataForKey", &UserSettings::setDataForKey)
        .endClass();

#endif //DISABLE_LUA_BINDINGS
}