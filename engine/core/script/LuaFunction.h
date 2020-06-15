#ifndef LUAFUNCTION_H
#define LUAFUNCTION_H

//
// (c) 2018 Eduardo Doria.
//

#include <string>

typedef struct lua_State lua_State;

namespace Supernova {

    class Object;
    class CollisionShape;

    class LuaFunction {

    private:
        int function;

    public:

        LuaFunction();
        LuaFunction(const LuaFunction& t);

        LuaFunction& operator = (const LuaFunction& t);

        bool operator == ( const LuaFunction& t ) const;
        operator bool() const;

        int set(lua_State *L);
        void remove();

        template<typename Ret, typename ...Args>
        Ret call(Args... args);

    };
}


#endif //LUAFUNCTION_H
