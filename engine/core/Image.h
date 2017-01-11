
#ifndef Image_h
#define Image_h

#include "Mesh2D.h"

void testando2();

class Image: public Mesh2D {

private:
    void createVertices();

public:
    Image();
    Image(int width, int height);
    Image(std::string image_path);
    virtual ~Image();

    bool load();

};


#endif /* Image_h */
