
#ifndef Rect_h
#define Rect_h

//
// (c) 2018 Eduardo Doria.
//

#include "Vector4.h"

namespace Supernova {

    class Rect{
    private:
        
        float x, y, width, height;
        
    public:
        
        Rect();
        Rect(float x, float y, float width, float height);
        Rect(const Rect& t);
        Rect(const Vector4& v);
        
        Rect& operator = (const Rect& t);
        Rect& operator = (const Vector4& v);
        bool operator == (const Rect& t);
        bool operator != (const Rect& t);
        
        float getX();
        float getY();
        float getWidth();
        float getHeight();

        Vector4 getVector();
        
        float* ptr();
        
        void setRect(float x, float y, float width, float height);
        void setRect(Rect rect);

        bool isNormalized();
    };
        
}

#endif /* Rect_h */
