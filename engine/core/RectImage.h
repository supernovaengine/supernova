#ifndef RectImage_h
#define RectImage_h

#include "Image.h"


class RectImage: public Image {
private:
    int texWidth;
    int texHeight;
    
    std::pair<int, int> slicePixelsPos;
    
    void updateSlicePosPixels();
    
protected:
    bool isSliced;
    int slicesX;
    int slicesY;
    
    int frame;
    
public:
    RectImage();
    virtual ~RectImage();
    
    void setFrame(int frame);
    void setSlices(int slicesX, int slicesY);
    
    bool load();
    bool draw();
};

#endif /* RectImage_h */
