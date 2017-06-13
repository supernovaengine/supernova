
#ifndef Image_h
#define Image_h

#include "Mesh2D.h"

class Image: public Mesh2D {

protected:
    void createVertices();

public:
    Image();
    Image(int width, int height);
    Image(std::string image_path);
    virtual ~Image();

    virtual void setSize(int width, int height);
    virtual void setInvert(bool invert);
    
    virtual bool load();

};


#endif /* Image_h */
