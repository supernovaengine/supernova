#ifndef PointRender_h
#define PointRender_h

#include "math/Vector2.h"
#include "render/SceneRender.h"
#include "render/TextureRender.h"
#include "math/Rect.h"
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
    std::vector<Rect*>* textureRects;
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

    virtual void updatePositions();
    virtual void updateNormals();
    virtual void updatePointSizes();
    virtual void updateTextureRects();
    virtual void updatePointColors();
    
    virtual bool load();
    virtual bool draw();
    virtual void destroy() = 0;
    
};


#endif /* PointRender_h */
