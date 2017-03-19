#ifndef SlicedImage_h
#define SlicedImage_h

#include "Image.h"


class SlicedImage: public Image {
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
    SlicedImage();
    virtual ~SlicedImage();
    
    void setFrame(int frame);
    void setSlices(int slicesX, int slicesY);
    
    bool load();
    bool draw();
};

#endif /* SlicedImage_h */
