
#ifndef ImageTemplate_h
#define ImageTemplate_h

#include "Mesh2D.h"
#include "Image.h"

class ImageTemplate: public Mesh2D {
private:

    std::vector<Image*> partImage;
    int rawImageWidth, rawImageHeight;
public:
    ImageTemplate();
    virtual ~ImageTemplate();

    bool load();
};

#endif /* ImageTemplate_h */
