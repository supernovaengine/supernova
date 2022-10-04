//
// (c) 2022 Eduardo Doria.
//

#include "LuaBinding.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaBridge.h"

#include "ecs/Entity.h"
#include "ecs/Signature.h"
#include "ecs/EntityManager.h"
#include "ecs/ComponentArray.h"

using namespace Supernova;

namespace luabridge
{
    // Entity type is not necessary here because it already exists Stack<unsigned int>

    template <>
    struct Stack <Signature>
    {
        static bool push(lua_State* L, Signature sig, std::error_code& ec)
        {
            lua_pushstring(L, sig.to_string().c_str());
            return true;
        }

        static Signature get(lua_State* L, int index)
        {
            return Signature(lua_tostring(L, index));
        }

        static bool isInstance (lua_State* L, int index)
        {
            return lua_type(L, index) == LUA_TSTRING;
        }
    };
}

void LuaBinding::registerECSClasses(lua_State *L){
#ifndef DISABLE_LUA_BINDINGS

    luabridge::getGlobalNamespace(L)
        .beginClass<EntityManager>("EntityManager")
        .addConstructor <void (*) (void)> ()
        .addFunction("createEntity", &EntityManager::createEntity)
        .addFunction("destroy", &EntityManager::destroy)
        .addFunction("setSignature", &EntityManager::setSignature)
        .addFunction("getSignature", &EntityManager::getSignature)
        .endClass();

#endif //DISABLE_LUA_BINDINGS
}