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
#include "io/UserSettings.h"

using namespace Supernova;

void LuaBinding::registerIOClasses(lua_State *L){
#ifndef DISABLE_LUA_BINDINGS

    sol::state_view lua(L);

    auto filedata = lua.new_usertype<FileData>("FileData",
        sol::no_constructor);

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


    auto file = lua.new_usertype<File>("File",
        sol::call_constructor, sol::constructors<File(), File(const char *, bool)>(),
        sol::base_classes, sol::bases<FileData>());


    auto data = lua.new_usertype<Data>("Data",
        sol::call_constructor, sol::constructors<Data(), Data(unsigned char *, unsigned int, bool, bool), Data(const char *)>(),
        sol::base_classes, sol::bases<FileData>());

    data["open"] = sol::overload(sol::resolve<unsigned int(unsigned char *, unsigned int, bool, bool)>(&Data::open), sol::resolve<unsigned int(const char *)>(&Data::open), sol::resolve<unsigned int(File *)>(&Data::open));


    auto usersettings = lua.new_usertype<UserSettings>("UserSettings",
        sol::no_constructor);

    usersettings["getBoolForKey"] = sol::overload(sol::resolve<bool(const char *)>(&UserSettings::getBoolForKey), sol::resolve<bool(const char *, bool)>(&UserSettings::getBoolForKey));
    usersettings["getIntegerForKey"] = sol::overload(sol::resolve<int(const char *)>(&UserSettings::getIntegerForKey), sol::resolve<int(const char *, int)>(&UserSettings::getIntegerForKey));
    usersettings["getLongForKey"] = sol::overload(sol::resolve<long(const char *)>(&UserSettings::getLongForKey), sol::resolve<long(const char *, long)>(&UserSettings::getLongForKey));
    usersettings["getFloatForKey"] = sol::overload(sol::resolve<float(const char *)>(&UserSettings::getFloatForKey), sol::resolve<float(const char *, float)>(&UserSettings::getFloatForKey));
    usersettings["getDoubleForKey"] = sol::overload(sol::resolve<double(const char *)>(&UserSettings::getDoubleForKey), sol::resolve<double(const char *, double)>(&UserSettings::getDoubleForKey));
    usersettings["getStringForKey"] = sol::overload(sol::resolve<std::string(const char *)>(&UserSettings::getStringForKey), sol::resolve<std::string(const char *, std::string)>(&UserSettings::getStringForKey));
    usersettings["getDataForKey"] = sol::overload(sol::resolve<Data(const char *)>(&UserSettings::getDataForKey), sol::resolve<Data(const char *, const Data&)>(&UserSettings::getDataForKey));
    usersettings["setBoolForKey"] = &UserSettings::setBoolForKey;
    usersettings["setIntegerForKey"] = &UserSettings::setIntegerForKey;
    usersettings["setLongForKey"] = &UserSettings::setLongForKey;
    usersettings["setFloatForKey"] = &UserSettings::setFloatForKey;
    usersettings["setDoubleForKey"] = &UserSettings::setDoubleForKey;
    usersettings["setStringForKey"] = &UserSettings::setStringForKey;
    usersettings["setDataForKey"] = &UserSettings::setDataForKey;

#endif //DISABLE_LUA_BINDINGS
}