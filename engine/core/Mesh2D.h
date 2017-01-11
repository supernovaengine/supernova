

#ifndef _Mesh2D_h
#define _Mesh2D_h

#include "Mesh.h"

class Mesh2D: public Mesh {
protected:
    int width;
    int height;
public:
    Mesh2D();
    ~Mesh2D();

    void setSize(int width, int height);

    void setWidth(int width);
    int getWidth();

    void setHeight(int height);
    int getHeight();
};

#endif /* _Mesh2D_hpp */
