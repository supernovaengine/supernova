//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "sol.hpp"

#include "Fog.h"

using namespace Supernova;

void LuaBinding::registerObjectEnvClasses(lua_State *L){
    sol::state_view lua(L);


    lua.new_enum("FogType",
                "LINEAR", FogType::LINEAR,
                "EXPONENTIAL", FogType::EXPONENTIAL,
                "EXPONENTIALSQUARED", FogType::EXPONENTIALSQUARED
                );

    lua.new_usertype<Fog>("Fog",
	        sol::default_constructor,
            "type", sol::property(&Fog::getType, &Fog::setType),
            "color", sol::property(&Fog::getColor, &Fog::setColor),
            "density", sol::property(&Fog::getDensity, &Fog::setDensity),
            "linearStart", sol::property(&Fog::getLinearStart, &Fog::setLinearStart),
            "linearEnd", sol::property(&Fog::getLinearEnd, &Fog::setLinearEnd),
            "setLinearStartEnd", &Fog::setLinearStartEnd
         );

}