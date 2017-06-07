

#ifndef _Mesh2D_h
#define _Mesh2D_h

#include "Mesh.h"

class Mesh2D: public Mesh {
protected:
    int width;
    int height;

    bool billboard;
    bool fixedSizeBillboard;
    float billboardScaleFactor;

    bool clipping;

public:
    Mesh2D();
    ~Mesh2D();

    virtual void setSize(int width, int height);
    
    void setBillboard(bool billboard);
    void setFixedSizeBillboard(bool fixedSizeBillboard);
    void setBillboardScaleFactor(float billboardScaleFactor);

    void setClipping(bool clipping);

    void setWidth(int width);
    int getWidth();

    void setHeight(int height);
    int getHeight();

    void update();
    void transform(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);

    bool draw();
};

#endif /* _Mesh2D_hpp */
