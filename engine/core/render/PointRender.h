#ifndef PointRender_h
#define PointRender_h

#include "math/Vector2.h"
#include "render/SceneRender.h"
#include "render/TextureRender.h"
#include "image/TextureRect.h"
#include "Material.h"
#include <vector>

class Points;


class PointRender {
    
private:
    
    void checkLighting();
    void checkFog();
    void checkTextureRect();
    void fillPointProperties();
    
protected:
    
    bool loaded;
    bool lighting;
    bool hasfog;
    bool hasTextureRect;
    
    Points* points;
    
    int numPoints;
    std::shared_ptr<TextureRender> texture;
    bool textured;
    
    SceneRender* sceneRender;
    
    std::vector<Vector3>* positions;
    std::vector<Vector3>* normals;
    std::vector<TextureRect*>* textureRects;
    std::vector<float>* pointSizes;
    std::vector<Vector4>* pointColors;
    
    Matrix4 modelMatrix;
    Matrix4 normalMatrix;
    Matrix4 modelViewProjectionMatrix;
    Vector3 cameraPosition;
    
    std::string materialTexture;

public:
    
    PointRender();
    virtual ~PointRender();
    
    void setPoints(Points* points);

    virtual void updatePositions() = 0;
    virtual void updateNormals() = 0;
    virtual void updatePointSizes() = 0;
    virtual void updateTextureRects() = 0;
    virtual void updatePointColors() = 0;
    
    virtual bool load();
    virtual bool draw();
    virtual void destroy() = 0;
    
};


#endif /* PointRender_h */
