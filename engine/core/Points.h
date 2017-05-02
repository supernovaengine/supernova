
#ifndef POINTS_H
#define POINTS_H

#define S_POINTSIZE_PIXEL 0
#define S_POINTSIZE_WIDTH 1
#define S_POINTSIZE_HEIGHT 2

#include "ConcreteObject.h"
#include "render/PointManager.h"
#include <unordered_map>

class Points: public ConcreteObject {

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
    Points();
    virtual ~Points();

    void transform(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
    void update();

    void setSizeAttenuation(bool sizeAttenuation);
    void setPointScaleFactor(float pointScaleFactor);
    void setPointSizeReference(int pointSizeReference);
    void setMinPointSize(float minPointSize);
    void setMaxPointSize(float maxPointSize);

    void addPoint();
    void addPoint(Vector3 position);

    void setPointPosition(int point, Vector3 position);
    void setPointPosition(int point, float x, float y, float z);
    void setPointSize(int point, float size);
    void setPointColor(int point, Vector4 color);
    void setPointColor(int point, float red, float green, float blue, float alpha);
    void setPointSprite(int point, std::string id);
    void setPointSprite(int point, int id);
    
    void addSpriteFrame(std::string id, float x, float y, float width, float height);
    void removeSpriteFrame(std::string id);
    
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
