
#ifndef GUIImage_h
#define GUIImage_h

//
// (c) 2018 Eduardo Doria.
//

#include "GUIObject.h"
#include "Image.h"

namespace Supernova {

    class GUIImage: public GUIObject {

    private:
        std::vector<Image*> partImage;
        int rawImageWidth, rawImageHeight;

    protected:
        int texWidth;
        int texHeight;
        
        int border_left;
        int border_right;
        int border_top;
        int border_bottom;

    public:
        GUIImage();
        virtual ~GUIImage();
        
        virtual void setSize(int width, int height);
        void setBorder(int border);
        
        void createVertices();

        virtual bool load();
    };
    
}

#endif /* GUIImage_h */
