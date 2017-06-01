

#ifndef luabinding_h
#define luabinding_h

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

class LuaBinding {
    
private:
    static int renderAPI;
public:
    LuaBinding();
    virtual ~LuaBinding();
    
    static int setLuaPath(const char* path);
    static int setLuaSearcher(lua_CFunction f, bool cleanSearchers = false);
    
    static int moduleLoader(lua_State *L);
    
    static void bind();
};

#endif /* luabinding_h */
