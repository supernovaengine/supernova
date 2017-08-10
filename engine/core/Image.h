
#ifndef Image_h
#define Image_h

#include "Mesh2D.h"

namespace Supernova {

    class Image: public Mesh2D {
        
    private:
        int texWidth;
        int texHeight;
        
        bool useTextureRect;
        
        Rect textureRect;

    protected:
        void createVertices();
        void normalizeTextureRect();

    public:
        Image();
        Image(int width, int height);
        Image(std::string image_path);
        virtual ~Image();
        
        void setTectureRect(float x, float y, float width, float height);
        void setTectureRect(Rect textureRect);

        virtual void setSize(int width, int height);
        virtual void setInvertTexture(bool invertTexture);
        
        virtual bool load();

    };
    
}


#endif /* Image_h */
