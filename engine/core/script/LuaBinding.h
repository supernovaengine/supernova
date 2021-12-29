//
// (c) 2020 Eduardo Doria.
//

#ifndef luabinding_h
#define luabinding_h

//#include "lua.h"

typedef struct lua_State lua_State;
typedef int (*lua_CFunction) (lua_State *L);

namespace Supernova {

    class LuaBinding {
        
        friend class Engine;
        
    private:
        static lua_State *luastate;
        
        static void createLuaState();
        static int setLuaPath(const char* path);
        static int setLuaSearcher(lua_CFunction f, bool cleanSearchers = false);
        
        static int moduleLoader(lua_State *L);
        static int handleLuaError(lua_State* L);
        static void registerClasses(lua_State *L);

        static void bind();

    public:
        LuaBinding();
        virtual ~LuaBinding();
        
        static lua_State* getLuaState();
        static void luaCallback(int nargs, int nresults, int msgh);
        static void stackDump (lua_State *L);
        
    };
    
}

#endif /* luabinding_h */
