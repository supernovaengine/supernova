
#ifndef PARTICLES_H
#define PARTICLES_H

#define S_POINTSIZE_PIXEL 0
#define S_POINTSIZE_WIDTH 1
#define S_POINTSIZE_HEIGHT 2

#include "ConcreteObject.h"
#include "render/PointManager.h"
#include <unordered_map>

class Particles: public ConcreteObject {

private:
    void updatePointScale();
    void fillScaledSizeVector();
    void normalizeTextureRects();
    
    std::vector<float> pointSizesScaled;
    
    int texWidth;
    int texHeight;

    float pointScale;

    int pointSizeReference;
    
    bool useTextureRects;

protected:
    PointManager renderManager;

    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<TextureRect> textureRects;
    std::vector<float> pointSizes;
    std::vector<Vector4> colors;

    bool sizeAttenuation;
    float pointScaleFactor;
    float minPointSize;
    float maxPointSize;
    
    std::unordered_map <std::string, TextureRect> framesRect;

public:
    Particles();
    virtual ~Particles();

    void transform(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
    void update();

    void setSizeAttenuation(bool sizeAttenuation);
    void setPointScaleFactor(float pointScaleFactor);
    void setPointSizeReference(int pointSizeReference);
    void setMinPointSize(float minPointSize);
    void setMaxPointSize(float maxPointSize);

    void addParticle();
    void addParticle(Vector3 position);

    void setParticlePosition(int particle, Vector3 position);
    void setParticlePosition(int particle, float x, float y, float z);
    void setParticleSize(int particle, float size);
    void setParticleColor(int particle, Vector4 color);
    void setParticleColor(int particle, float red, float green, float blue, float alpha);
    void setParticleFrame(int particle, std::string id);
    void setParticleFrame(int particle, int id);
    
    void addFrame(std::string id, float x, float y, float width, float height);
    void removeFrame(std::string id);
    
    std::vector<Vector3>* getPositions();
    std::vector<Vector3>* getNormals();
    std::vector<TextureRect>* getTextureRects();
    std::vector<float>* getPointSizes();
    std::vector<Vector4>* getColors();

    bool render();
    bool load();
    bool draw();
};


#endif
