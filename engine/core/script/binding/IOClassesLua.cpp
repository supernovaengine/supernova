//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "sol.hpp"

using namespace Supernova;

void LuaBinding::registerIOClasses(lua_State *L){
    sol::state_view lua(L);
}