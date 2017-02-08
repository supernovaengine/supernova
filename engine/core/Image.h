
#ifndef Image_h
#define Image_h

#include "Mesh2D.h"

class Image: public Mesh2D {

private:
    void createVertices();

public:
    Image();
    Image(int width, int height);
    Image(std::string image_path);
    virtual ~Image();

    void update();
    void transform(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);

    bool load();

};


#endif /* Image_h */
