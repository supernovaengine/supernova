#ifndef DrawRender_h
#define DrawRender_h

#include "math/Vector2.h"
#include "render/SceneRender.h"
#include "Submesh.h"
#include <vector>


class DrawRender {
    
protected:
    
    SceneRender* sceneRender;
    
    std::vector<Vector3>* positions;
    std::vector<Vector3>* normals;
    std::vector<Vector2>* texcoords;
    std::vector<Submesh*>* submeshes;
    
    Matrix4* modelMatrix;
    Matrix4* normalMatrix;
    Matrix4* modelViewProjectionMatrix;
    Vector3* cameraPosition;
    
    Material* material;

    bool isPoints;
    bool isSky;
    bool isSpriteSheet;

    int textureSizeWidth;
    int textureSizeHeight;
    
    int spriteSizeWidth;
    int spriteSizeHeight;

    int spritePosX;
    int spritePosY;
    
    std::vector< std::pair<int, int> >* pointSpritesPos;
    
    std::vector<float>* pointSizes;
    
    std::vector<Vector4>* pointColors;

    int pointSize;
    
    int primitiveMode;
public:
    
    DrawRender();
    virtual ~DrawRender();
    
    void setSceneRender(SceneRender* sceneRender);
    void setPositions(std::vector<Vector3>* positions);
    void setNormals(std::vector<Vector3>* normals);
    void setTexcoords(std::vector<Vector2>* texcoords);
    void setSubmeshes(std::vector<Submesh*>* submeshes);
    void setModelMatrix(Matrix4* modelMatrix);
    void setNormalMatrix(Matrix4* normalMatrix);
    void setModelViewProjectionMatrix( Matrix4* modelViewProjectionMatrix);
    void setCameraPosition(Vector3* cameraPosition);
    void setPrimitiveMode(int primitiveMode);
    void setMaterial(Material* material);
    void setIsPoints(bool isPoints);
    void setIsSky(bool isSky);
    void setPointSizes(std::vector<float>* pointSizes);
    void setIsSpriteSheet(bool isSpriteSheet);
    void setTextureSize(int textureSizeWidth, int textureSizeHeight);
    void setSpriteSize(int spriteSizeWidth, int spriteSizeHeight);
    void setSpritePos(int spritePosX, int spritePosY);
    void setPointSpritesPos(std::vector< std::pair<int, int> >* pointSpritesPos);
    void setPointColors(std::vector<Vector4>* pointColors);
    
    virtual void updatePositions() = 0;
    virtual void updateTexcoords() = 0;
    virtual void updateNormals() = 0;
    virtual void updatePointSizes() = 0;
    virtual void updateSpritePos() = 0;
    virtual void updatePointColors() = 0;
    
    virtual bool load() = 0;
    virtual bool draw() = 0;
    virtual void destroy() = 0;
    
};

#endif /* DrawRender_h */
