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
        
        static void (*onTouchPressFunc)(float, float);
        static int onTouchPressLuaFunc;
        
        static void (*onTouchUpFunc)(float, float);
        static int onTouchUpLuaFunc;
        
        static void (*onTouchDragFunc)(float, float);
        static int onTouchDragLuaFunc;
        
        static void (*onMousePressFunc)(int, float, float);
        static int onMousePressLuaFunc;
        
        static void (*onMouseUpFunc)(int, float, float);
        static int onMouseUpLuaFunc;
        
        static void (*onMouseDragFunc)(int, float, float);
        static int onMouseDragLuaFunc;
        
        static void (*onMouseMoveFunc)(float, float);
        static int onMouseMoveLuaFunc;
        
        static void (*onKeyPressFunc)(int);
        static int onKeyPressLuaFunc;
        
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
        
        static void onTouchPress(void (*onTouchPressFunc)(float, float));
        static int onTouchPress(lua_State *L);
        static void call_onTouchPress(float x, float y);
        
        static void onTouchUp(void (*onTouchUpFunc)(float, float));
        static int onTouchUp(lua_State *L);
        static void call_onTouchUp(float x, float y);
        
        static void onTouchDrag(void (*onTouchDragFunc)(float, float));
        static int onTouchDrag(lua_State *L);
        static void call_onTouchDrag(float x, float y);
        
        static void onMousePress(void (*onMousePressFunc)(int, float, float));
        static int onMousePress(lua_State *L);
        static void call_onMousePress(int button, float x, float y);
        
        static void onMouseUp(void (*onMouseUpFunc)(int, float, float));
        static int onMouseUp(lua_State *L);
        static void call_onMouseUp(int button, float x, float y);
        
        static void onMouseDrag(void (*onMouseDragFunc)(int, float, float));
        static int onMouseDrag(lua_State *L);
        static void call_onMouseDrag(int button, float x, float y);
        
        static void onMouseMove(void (*onMouseMoveFunc)(float, float));
        static int onMouseMove(lua_State *L);
        static void call_onMouseMove(float x, float y);
        
        static void onKeyPress(void (*onKeyPressFunc)(int));
        static int onKeyPress(lua_State *L);
        static void call_onKeyPress(int key);
        
        static void onKeyUp(void (*onKeyUpFunc)(int));
        static int onKeyUp(lua_State *L);
        static void call_onKeyUp(int key);
        
    };
    
}

#endif /* Events_h */
