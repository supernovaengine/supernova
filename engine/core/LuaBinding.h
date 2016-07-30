

#ifndef luabinding_h
#define luabinding_h

class LuaBinding {
    
private:
    static int renderAPI;
public:
    LuaBinding();
    virtual ~LuaBinding();
    
    static int setLuaPath(const char* path);
    static void bind();
};

#endif /* luabinding_h */
