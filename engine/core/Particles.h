
#ifndef PARTICLES_H
#define PARTICLES_H

#include "ConcreteObject.h"
#include "render/DrawManager.h"

class Particles: public ConcreteObject {

private:
    void updatePointScale();
    void fillScaledSizeVector();
    std::vector<float> pointSizesScaled;

protected:
    DrawManager renderManager;

    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<float> pointSizes;

    float pointScale;
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
    void setSizeAttenuation(bool sizeAttenuation);
    void setPointScaleFactor(float pointScaleFactor);
    void setMinPointSize(float minPointSize);
    void setMaxPointSize(float maxPointSize);

    void addParticle();
    void addParticle(Vector3 position);

    void setParticlePosition(int particle, Vector3 position);
    void setParticlePosition(int particle, float x, float y, float z);
    void setParticleSize(int particle, float size);

    bool render();
    bool load();
    bool draw();
};


#endif
