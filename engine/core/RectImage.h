#ifndef RectImage_h
#define RectImage_h

#include "Image.h"
#include "image/TextureRect.h"


class RectImage: public Image {
private:
    int texWidth;
    int texHeight;

    TextureRect textureRect;
    //TextureRect normalizedRect;
    
protected:
    bool isSliced;

    void generateNormalizateRect();
    
public:
    RectImage();
    virtual ~RectImage();

    void setRect(float x, float y, float width, float height);
    void setRect(TextureRect textureRect);
    
    bool load();
    bool draw();
};

#endif /* RectImage_h */
