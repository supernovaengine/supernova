
#ifndef Image_h
#define Image_h

#include "Mesh2D.h"

namespace Supernova {

    class Image: public Mesh2D {
        
    private:
        InterleavedBuffer buffer;
        IndexBuffer indices;

        bool useTextureRect;
        Rect textureRect;

    protected:
        int texWidth;
        int texHeight;

        void createVertices();
        void normalizeTextureRect();

    public:
        Image();
        Image(int width, int height);
        Image(std::string image_path);
        virtual ~Image();
        
        void setTextureRect(float x, float y, float width, float height);
        void setTextureRect(Rect textureRect);
        Rect getTextureRect();

        virtual void setSize(int width, int height);
        virtual void setInvertTexture(bool invertTexture);
        
        virtual bool load();

    };
    
}


#endif /* Image_h */
