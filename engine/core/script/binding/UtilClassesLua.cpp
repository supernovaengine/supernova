//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "util/Angle.h"
#include "util/Base64.h"
#include "util/Color.h"

using namespace Supernova;

void LuaBinding::registerUtilClasses(lua_State *L){
#ifndef DISABLE_LUA_BINDINGS
/*
    sol::state_view lua(L);

    auto angle = lua.new_usertype<Angle>("Angle",
        sol::no_constructor);

    angle["radToDefault"] = Angle::radToDefault;
    angle["degToDefault"] = Angle::degToDefault;
    angle["defaultToRad"] = Angle::defaultToRad;
    angle["defaultToDeg"] = Angle::defaultToDeg;
    angle["radToDeg"] = Angle::radToDeg;
    angle["degToRad"] = Angle::degToRad;

    auto base64 = lua.new_usertype<Base64>("Base64",
        sol::no_constructor);

    base64["encode"] = Base64::encode;
    base64["decode"] = Base64::decode;

    auto color = lua.new_usertype<Color>("Color",
        sol::no_constructor);

    color["linearTosRGB"] = sol::overload(sol::resolve<Vector3(Vector3)>(Color::linearTosRGB), sol::resolve<Vector4(Vector4)>(Color::linearTosRGB));
    color["sRGBToLinear"] = sol::overload(sol::resolve<Vector3(Vector3)>(Color::sRGBToLinear), sol::resolve<Vector4(Vector4)>(Color::sRGBToLinear));
*/
#endif //DISABLE_LUA_BINDINGS
}