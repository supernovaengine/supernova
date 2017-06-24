

#ifndef luabind_h
#define luabind_h

#include "Object.h"

typedef struct lua_State lua_State;
typedef int (*lua_CFunction) (lua_State *L);

namespace Supernova {

    class LuaBind {
        
        friend class Engine;
        
    private:
        static lua_State *luastate;
        
        static void createLuaState();
        static int setLuaPath(const char* path);
        static int setLuaSearcher(lua_CFunction f, bool cleanSearchers = false);
        
        static int moduleLoader(lua_State *L);
        
        static void bind();
    public:
        LuaBind();
        virtual ~LuaBind();
        
        static lua_State* getLuaState();
        
        static void setObject(const char* global, Object* object);
        static Object* getObject(const char* global);
        
    };
    
}

#endif /* luabind_h */
