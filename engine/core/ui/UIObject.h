
#ifndef UIObject_h
#define UIObject_h

//
// (c) 2018 Eduardo Doria.
//

#include "Mesh2D.h"
#include "util/FunctionSubscribe.h"

namespace Supernova {

    class UIObject: public Mesh2D {
    protected:
        int state;
        int pointerDown;
        bool focused;

        std::string eventId;

        bool isCoordInside(float x, float y);

    public:
        UIObject();
        virtual ~UIObject();
        
        int getState();

        FunctionSubscribe<void()> onDown;
        FunctionSubscribe<void()> onUp;

        virtual void inputDown();
        virtual void inputUp();

        virtual void getFocus();
        virtual void lostFocus();

        virtual void engineOnDown(int pointer, float x, float y);
        virtual void engineOnUp(int pointer, float x, float y);
        virtual void engineOnTextInput(std::string text);

        virtual bool load();
        virtual void destroy();
    };
    
}

#endif /* UIObject_h */
