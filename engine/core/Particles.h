
#ifndef PARTICLES_H
#define PARTICLES_H

#include "ConcreteObject.h"
#include "render/DrawManager.h"

class Particles: public ConcreteObject {

private:
    void updatePointDistance();

protected:
    DrawManager renderManager;

    std::vector<Vector3> positions;
    std::vector<Vector3> normals;

    float pointScale;
    float pointSize;
    bool sizeAttenuation;
    float pointScaleFactor;

    float minPointSize;
    float maxPointSize;

public:
    Particles();
    virtual ~Particles();

    void transform(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
    void update();

    void setPointScale(float pointScale);
    void setPointSize(float pointSize);
    void setSizeAttenuation(bool sizeAttenuation);
    void setPointScaleFactor(float pointScaleFactor);
    void setMinPointSize(float minPointSize);
    void setMaxPointSize(float maxPointSize);

    bool render();
    bool load();
    bool draw();
};


#endif
