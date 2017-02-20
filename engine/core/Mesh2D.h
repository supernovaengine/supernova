

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

public:
    Mesh2D();
    ~Mesh2D();

    void setSize(int width, int height);
    void setBillboard(bool billboard);
    void setFixedSizeBillboard(bool fixedSizeBillboard);
    void setBillboardScaleFactor(float billboardScaleFactor);

    void setWidth(int width);
    int getWidth();

    void setHeight(int height);
    int getHeight();

    void update();
    void transform(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
};

#endif /* _Mesh2D_hpp */
