#ifndef Button_h
#define Button_h

#include "GUIImage.h"

namespace Supernova {

    class Button: public GUIImage {
    public:
        Button();
        virtual ~Button();

        virtual void call_onPress();
    };
    
}

#endif /* Button_h */
