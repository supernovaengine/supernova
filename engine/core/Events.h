#ifndef Events_h
#define Events_h

typedef struct lua_State lua_State;

namespace Supernova {

    class Events {
        
    private:
        
        static void (*onDrawFunc)();
        static int onDrawLuaFunc;
        
        static void (*onUpdateFunc)();
        static int onUpdateLuaFunc;
        
        static void (*onTouchStartFunc)(float, float);
        static int onTouchStartLuaFunc;
        
        static void (*onTouchEndFunc)(float, float);
        static int onTouchEndLuaFunc;
        
        static void (*onTouchDragFunc)(float, float);
        static int onTouchDragLuaFunc;
        
        static void (*onMouseDownFunc)(int, float, float);
        static int onMouseDownLuaFunc;
        
        static void (*onMouseUpFunc)(int, float, float);
        static int onMouseUpLuaFunc;
        
        static void (*onMouseDragFunc)(int, float, float);
        static int onMouseDragLuaFunc;
        
        static void (*onMouseMoveFunc)(float, float);
        static int onMouseMoveLuaFunc;
        
        static void (*onKeyDownFunc)(int);
        static int onKeyDownLuaFunc;
        
        static void (*onKeyUpFunc)(int);
        static int onKeyUpLuaFunc;
        
    public:
        Events();
        virtual ~Events();
        
        static void onDraw(void (*onDrawFunc)());
        static int onDraw(lua_State *L);
        static void call_onDraw();
        
        static void onUpdate(void (*onUpdateFunc)());
        static int onUpdate(lua_State *L);
        static void call_onUpdate();
        
        static void onTouchStart(void (*onTouchStartFunc)(float, float));
        static int onTouchStart(lua_State *L);
        static void call_onTouchStart(float x, float y);
        
        static void onTouchEnd(void (*onTouchEndFunc)(float, float));
        static int onTouchEnd(lua_State *L);
        static void call_onTouchEnd(float x, float y);
        
        static void onTouchDrag(void (*onTouchDragFunc)(float, float));
        static int onTouchDrag(lua_State *L);
        static void call_onTouchDrag(float x, float y);
        
        static void onMouseDown(void (*onMouseDownFunc)(int, float, float));
        static int onMouseDown(lua_State *L);
        static void call_onMouseDown(int button, float x, float y);
        
        static void onMouseUp(void (*onMouseUpFunc)(int, float, float));
        static int onMouseUp(lua_State *L);
        static void call_onMouseUp(int button, float x, float y);
        
        static void onMouseDrag(void (*onMouseDragFunc)(int, float, float));
        static int onMouseDrag(lua_State *L);
        static void call_onMouseDrag(int button, float x, float y);
        
        static void onMouseMove(void (*onMouseMoveFunc)(float, float));
        static int onMouseMove(lua_State *L);
        static void call_onMouseMove(float x, float y);
        
        static void onKeyDown(void (*onKeyDownFunc)(int));
        static int onKeyDown(lua_State *L);
        static void call_onKeyDown(int key);
        
        static void onKeyUp(void (*onKeyUpFunc)(int));
        static int onKeyUp(lua_State *L);
        static void call_onKeyUp(int key);
        
    };
    
}

#endif /* Events_h */
