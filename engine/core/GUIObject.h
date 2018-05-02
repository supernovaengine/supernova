
#ifndef GUIObject_h
#define GUIObject_h

#include "Mesh2D.h"

namespace Supernova {

    class GUIObject: public Mesh2D {
    private:
        void (*onDownFunc)();
        int onDownLuaFunc;
        
        void (*onUpFunc)();
        int onUpLuaFunc;

    protected:
        int state;

        void call_onDown();
        void call_onUp();
        
    public:
        GUIObject();
        virtual ~GUIObject();
        
        int getState();

        void onDown(void (*onDownFunc)());
        int onDown(lua_State *L);
        
        void onUp(void (*onUpFunc)());
        int onUp(lua_State *L);

        bool isCoordInside(float x, float y);

        virtual void engine_onDown(float x, float y);
        virtual void engine_onUp(float x, float y);
        virtual void engine_onTextInput(std::string text);
    };
    
}

#endif /* GUIObject_h */
