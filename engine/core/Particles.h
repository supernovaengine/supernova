
#ifndef PARTICLES_H
#define PARTICLES_H

#define S_POINT_SCALING_PIXEL 0
#define S_POINT_SCALING_WIDTH 1
#define S_POINT_SCALING_HEIGHT 2

#include "ConcreteObject.h"
#include "render/PointManager.h"

class Particles: public ConcreteObject {

private:
    void updatePointScale();
    void fillScaledSizeVector();
    void fillSpritePosPixelsVector();
    
    std::vector<float> pointSizesScaled;
    std::vector< std::pair<int, int> > spritesPixelsPos;
    
    int texWidth;
    int texHeight;

protected:
    PointManager renderManager;

    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<float> pointSizes;
    std::vector<int> sprites;
    std::vector<Vector4> colors;

    float pointScale;

    bool sizeAttenuation;
    float pointScaleFactor;
    int pointScalingMode;
    float minPointSize;
    float maxPointSize;

    bool isSpriteSheet;
    int spritesX;
    int spritesY;

public:
    Particles();
    virtual ~Particles();

    void transform(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
    void update();

    void setSizeAttenuation(bool sizeAttenuation);
    void setPointScaleFactor(float pointScaleFactor);
    void setPointScalingMode(int pointScalingMode);
    void setMinPointSize(float minPointSize);
    void setMaxPointSize(float maxPointSize);

    void addParticle();
    void addParticle(Vector3 position);

    void setParticlePosition(int particle, Vector3 position);
    void setParticlePosition(int particle, float x, float y, float z);
    void setParticleSize(int particle, float size);
    void setParticleSprite(int particle, int sprite);
    void setParticleColor(int particle, Vector4 color);
    void setParticleColor(int particle, float red, float green, float blue, float alpha);

    void setSpriteSheet(int spritesX, int spritesY);

    bool render();
    bool load();
    bool draw();
};


#endif
