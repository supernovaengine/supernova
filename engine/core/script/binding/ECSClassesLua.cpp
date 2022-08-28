//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "sol.hpp"

#include "ecs/Entity.h"
#include "ecs/Signature.h"
#include "ecs/EntityManager.h"
#include "ecs/ComponentArray.h"

using namespace Supernova;

void LuaBinding::registerECSClasses(lua_State *L){
    sol::state_view lua(L);

    lua.new_usertype<Entity>("Entity");

    lua.new_usertype<Signature>("Signature");

    lua.new_usertype<EntityManager>("EntityManager",
        "createEntity", &EntityManager::createEntity,
        "destroy", &EntityManager::destroy,
        "setSignature", &EntityManager::setSignature,
        "setSignature", &EntityManager::setSignature
    );
}