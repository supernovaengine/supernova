
#ifndef UIObject_h
#define UIObject_h

//
// (c) 2018 Eduardo Doria.
//

#include "Mesh2D.h"
#include "util/FunctionCallback.h"

namespace Supernova {

    class UIObject: public Mesh2D {
    protected:
        int state;
        int pointerDown;
        bool focused;

        bool isCoordInside(float x, float y);

    public:
        UIObject();
        virtual ~UIObject();
        
        int getState();

        FunctionCallback<void()> onDown;
        FunctionCallback<void()> onUp;

        virtual void inputDown();
        virtual void inputUp();

        virtual void getFocus();
        virtual void lostFocus();

        virtual void engineOnDown(int pointer, float x, float y);
        virtual void engineOnUp(int pointer, float x, float y);
        virtual void engineOnTextInput(std::string text);
    };
    
}

#endif /* UIObject_h */
