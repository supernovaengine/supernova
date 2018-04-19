
#ifndef GUIObject_h
#define GUIObject_h

#include "Mesh2D.h"

namespace Supernova {

    class GUIObject: public Mesh2D {
    private:

        void (*onPressFunc)();
        int onPressLuaFunc;

    public:
        GUIObject();
        virtual ~GUIObject();

        void onPress(void (*onPressFunc)());
        int onPress(lua_State *L);
        virtual void call_onPress();
    };
    
}

#endif /* GUIObject_h */
