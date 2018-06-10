#ifndef LUAFUNCTION_H
#define LUAFUNCTION_H

#include "Object.h"

//
// (c) 2018 Eduardo Doria.
//

typedef struct lua_State lua_State;

namespace Supernova {

    class LuaFunction {

    private:
        int function;

    public:

        LuaFunction();
        LuaFunction(const LuaFunction& t);

        LuaFunction& operator = (const LuaFunction& t);

        int set(lua_State *L);
        void reset();

        void call();
        void call(int p1);
        void call(int p1, int p2);
        void call(float p1);
        void call(float p1, float p2);
        void call(int p1, float p2, float p3);
        void call(Object* p1);
        void call(std::string p1);

    };
}


#endif //LUAFUNCTION_H
