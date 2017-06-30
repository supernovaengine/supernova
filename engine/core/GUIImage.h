
#ifndef GUIImage_h
#define GUIImage_h

#include "GUIObject.h"
#include "Image.h"

namespace Supernova {

    class GUIImage: public GUIObject {
    private:

        std::vector<Image*> partImage;
        int rawImageWidth, rawImageHeight;
    public:
        GUIImage();
        virtual ~GUIImage();
        
        void setSize(int width, int height);
        void createVertices();

        bool load();
    };
    
}

#endif /* GUIImage_h */
