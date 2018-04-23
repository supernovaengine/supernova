
#ifndef GUIObject_h
#define GUIObject_h

#include "Mesh2D.h"

namespace Supernova {

    class GUIObject: public Mesh2D {
    private:
        void (*onPressFunc)();
        int onPressLuaFunc;
        
        void (*onUpFunc)();
        int onUpLuaFunc;

    protected:
        int state;
        
    public:
        GUIObject();
        virtual ~GUIObject();
        
        int getState();

        void onPress(void (*onPressFunc)());
        int onPress(lua_State *L);
        virtual void call_onPress();
        
        void onUp(void (*onUpFunc)());
        int onUp(lua_State *L);
        virtual void call_onUp();
    };
    
}

#endif /* GUIObject_h */
