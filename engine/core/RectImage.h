#ifndef RectImage_h
#define RectImage_h

#include "Image.h"
#include "math/Rect.h"

namespace Supernova {

    class RectImage: public Image {
    private:
        int texWidth;
        int texHeight;
        
        bool useTextureRect;

        Rect textureRect;
        
    protected:

        void normalizeTextureRect();
        
    public:
        RectImage();
        virtual ~RectImage();

        void setRect(float x, float y, float width, float height);
        void setRect(Rect textureRect);
        
        bool load();
    };
    
}

#endif /* RectImage_h */
