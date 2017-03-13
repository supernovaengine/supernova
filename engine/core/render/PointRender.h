#ifndef PointRender_h
#define PointRender_h

#include "math/Vector2.h"
#include "render/SceneRender.h"
#include "render/TextureRender.h"
#include "Material.h"
#include <vector>


class PointRender {
    
private:
    
    void checkLighting();
    
protected:
    
    bool loaded;
    bool lighting;
    
    int numPoints;
    std::shared_ptr<TextureRender> texture;
    bool textured;
    
    SceneRender* sceneRender;
    
    std::vector<Vector3>* positions;
    std::vector<Vector3>* normals;
    std::vector< std::pair<int, int> >* pointSpritesPos;
    std::vector<float>* pointSizes;
    std::vector<Vector4>* pointColors;
    
    Matrix4* modelMatrix;
    Matrix4* normalMatrix;
    Matrix4* modelViewProjectionMatrix;
    Vector3* cameraPosition;
    
    Material* material;
    
    bool isSpriteSheet;
    
    int textureSizeWidth;
    int textureSizeHeight;
    
    int spriteSizeWidth;
    int spriteSizeHeight;

public:
    
    PointRender();
    virtual ~PointRender();
    
    void setSceneRender(SceneRender* sceneRender);
    void setPositions(std::vector<Vector3>* positions);
    void setNormals(std::vector<Vector3>* normals);
    void setModelMatrix(Matrix4* modelMatrix);
    void setNormalMatrix(Matrix4* normalMatrix);
    void setModelViewProjectionMatrix( Matrix4* modelViewProjectionMatrix);
    void setCameraPosition(Vector3* cameraPosition);
    void setMaterial(Material* material);
    void setPointSizes(std::vector<float>* pointSizes);
    void setIsSpriteSheet(bool isSpriteSheet);
    void setTextureSize(int textureSizeWidth, int textureSizeHeight);
    void setSpriteSize(int spriteSizeWidth, int spriteSizeHeight);
    void setPointSpritesPos(std::vector< std::pair<int, int> >* pointSpritesPos);
    void setPointColors(std::vector<Vector4>* pointColors);
    
    virtual void updatePositions() = 0;
    virtual void updateNormals() = 0;
    virtual void updatePointSizes() = 0;
    virtual void updateSpritePos() = 0;
    virtual void updatePointColors() = 0;
    
    virtual bool load();
    virtual bool draw();
    virtual void destroy() = 0;
    
};


#endif /* PointRender_h */
