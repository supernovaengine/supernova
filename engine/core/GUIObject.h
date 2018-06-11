
#ifndef GUIObject_h
#define GUIObject_h

#include "Mesh2D.h"
#include "util/FunctionCallback.h"

namespace Supernova {

    class GUIObject: public Mesh2D {
    protected:
        int state;
        
    public:
        GUIObject();
        virtual ~GUIObject();
        
        int getState();

        FunctionCallback<void()> onDown;
        FunctionCallback<void()> onUp;

        bool isCoordInside(float x, float y);

        virtual void engineOnDown(int pointer, float x, float y);
        virtual void engineOnUp(int pointer, float x, float y);
        virtual void engineOnTextInput(std::string text);
    };
    
}

#endif /* GUIObject_h */
