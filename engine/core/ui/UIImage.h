
#ifndef UIImage_h
#define UIImage_h

//
// (c) 2018 Eduardo Doria.
//

#include "UIObject.h"
#include "Image.h"

namespace Supernova {

    class UIImage: public UIObject {

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
        UIImage();
        virtual ~UIImage();
        
        virtual void setSize(int width, int height);
        void setBorder(int border);
        
        void createVertices();

        virtual bool load();
    };
    
}

#endif /* UIImage_h */
