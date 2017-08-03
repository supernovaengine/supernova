
#ifndef Rect_h
#define Rect_h

namespace Supernova {

    class Rect{
    private:
        
        float x, y, width, height;
        
    public:
        
        Rect();
        Rect(float x, float y, float width, float height);
        Rect(const Rect& t);
        
        Rect& operator = (const Rect& t);
        
        float getX();
        float getY();
        float getWidth();
        float getHeight();
        
        float* ptr();
        
        void setRect(float x, float y, float width, float height);
        void setRect(Rect* rect);

        bool isNormalized();
    };
        
}

#endif /* Rect_h */
