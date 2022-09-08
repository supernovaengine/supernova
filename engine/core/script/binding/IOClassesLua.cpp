//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "sol.hpp"

#include "io/Data.h"
#include "io/File.h"
#include "io/FileData.h"

using namespace Supernova;

void LuaBinding::registerIOClasses(lua_State *L){
    sol::state_view lua(L);
/*
    auto filedata = lua.new_usertype<FileData>("FileData",
        sol::call_constructor, sol::default_constructor);

    filedata["newFile"] = sol::overload(sol::resolve<FileData*(bool)>(FileData::newFile), sol::resolve<FileData*(const char *, bool)>(FileData::newFile));
    filedata["newFile"] = &FileData::getBaseDir;
    filedata["getFilePathExtension"] = &FileData::getFilePathExtension;
    filedata["getSystemPath"] = &FileData::getSystemPath;
    filedata["read8"] = &FileData::read8;
    filedata["read16"] = &FileData::read16;
    filedata["read32"] = &FileData::read32;
    filedata["eof"] = &FileData::eof;
    filedata["read"] = &FileData::read;
    filedata["write"] = &FileData::write;
    filedata["length"] = &FileData::length;
    filedata["seek"] = &FileData::seek;
    filedata["pos"] = &FileData::pos;
    filedata["readString"] = &FileData::readString;
    filedata["writeString"] = &FileData::writeString;
    */
}