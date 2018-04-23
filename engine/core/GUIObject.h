
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

        void call_onPress();
        void call_onUp();
        
    public:
        GUIObject();
        virtual ~GUIObject();
        
        int getState();

        void onPress(void (*onPressFunc)());
        int onPress(lua_State *L);
        
        void onUp(void (*onUpFunc)());
        int onUp(lua_State *L);

        bool isCoordInside(float x, float y);

        virtual void engine_onPress(float x, float y);
        virtual void engine_onUp(float x, float y);
        virtual void engine_onTextInput(const char* text);
    };
    
}

#endif /* GUIObject_h */
