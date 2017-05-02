
#ifndef GUIImage_h
#define GUIImage_h

#include "Mesh2D.h"
#include "Image.h"

class GUIImage: public Mesh2D {
private:

    std::vector<Image*> partImage;
    int rawImageWidth, rawImageHeight;
public:
    GUIImage();
    virtual ~GUIImage();

    bool load();
};

#endif /* GUIImage_h */
