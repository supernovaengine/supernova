//
// (c) 2024 Eduardo Doria.
//

#ifndef luabinding_h
#define luabinding_h

#include "Export.h"

typedef struct lua_State lua_State;
typedef int (*lua_CFunction) (lua_State *L);

namespace Supernova {

    class SUPERNOVA_API LuaBinding {
        
        friend class Engine;
        
    private:
        static lua_State *luastate;
        
        static void createLuaState();
        static int setLuaPath(const char* path);
        static int setLuaSearcher(lua_CFunction f, bool cleanSearchers = false);
        
        static int moduleLoader(lua_State *L);
        static int handleLuaError(lua_State* L);
        static void registerClasses(lua_State *L);

        // in binding directory
        static void registerActionClasses(lua_State *L);
        static void registerCoreClasses(lua_State *L);
        static void registerECSClasses(lua_State *L);
        static void registerIOClasses(lua_State *L);
        static void registerMathClasses(lua_State *L);
        static void registerObjectClasses(lua_State *L);
        static void registerUtilClasses(lua_State *L);
        static void registerThreadClasses(lua_State *L);

        static void init();

    public:
        LuaBinding();
        virtual ~LuaBinding();
        
        static lua_State* getLuaState();
        
        // old lua function call
        static void luaCallback(int nargs, int nresults, int msgh);

        static void stackDump (lua_State *L);

        static void cleanup();
        
    };
    
}

#endif /* luabinding_h */
