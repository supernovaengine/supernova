
#ifndef GUIObject_h
#define GUIObject_h

#include "Mesh2D.h"

namespace Supernova {

    class GUIObject: public Mesh2D {
    private:

    public:
        GUIObject();
        virtual ~GUIObject();
        
        void onTouchPress(float x, float y);
    };
    
}

#endif /* GUIObject_h */
