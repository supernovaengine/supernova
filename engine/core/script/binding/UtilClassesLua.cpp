//
// (c) 2024 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.hpp"

#include "LuaBridge.h"

#include "util/Angle.h"
#include "util/Base64.h"
#include "util/Color.h"

using namespace Supernova;

void LuaBinding::registerUtilClasses(lua_State *L){
#ifndef DISABLE_LUA_BINDINGS

    luabridge::getGlobalNamespace(L)
        .beginClass<Angle>("Angle")
        .addStaticFunction("radToDefault", &Angle::radToDefault)
        .addStaticFunction("degToDefault", &Angle::degToDefault)
        .addStaticFunction("defaultToRad", &Angle::defaultToRad)
        .addStaticFunction("defaultToDeg", &Angle::defaultToDeg)
        .addStaticFunction("radToDeg", &Angle::radToDeg)
        .addStaticFunction("degToRad", &Angle::degToRad)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Base64>("Base64")
        //.addStaticFunction("encode", &Base64::encode)  //TODO: read and write data in Lua
        //.addStaticFunction("decode", &Base64::decode)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Color>("Color")
        .addStaticFunction("linearTosRGB", (Vector4(*)(Vector4))&Color::linearTosRGB)
        .addStaticFunction("sRGBToLinear", (Vector4(*)(Vector4))&Color::sRGBToLinear)
        .endClass();

#endif //DISABLE_LUA_BINDINGS
}