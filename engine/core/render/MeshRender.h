#ifndef MeshRender_h
#define MeshRender_h

#include "math/Vector2.h"
#include "render/SceneRender.h"
#include "render/TextureRender.h"
#include "Submesh.h"
#include <vector>
#include <unordered_map>
#include "image/TextureRect.h"


class MeshRender {

    typedef struct {
        std::shared_ptr<TextureRender> texture;
        unsigned int indicesSizes;
        bool textured;
    } SubmeshStruct;

private:

    void checkLighting();
    void checkFog();
    
protected:

    bool loaded;
    bool lighting;
    bool hasfog;

    std::unordered_map<Submesh*, SubmeshStruct> submeshesGles;
    
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

    bool isSky;
    bool isRectImage;

    TextureRect* textureRect;
/*
    int textureSizeWidth;
    int textureSizeHeight;
    
    int rectSizeWidth;
    int rectSizeHeight;

    std::pair<int, int>* rectPos;
*/
    int primitiveMode;
public:
    
    MeshRender();
    virtual ~MeshRender();
    
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
    void setIsSky(bool isSky);
    void setIsRectImage(bool isRectImage);
    void setTextureRect(TextureRect* textureRect);
    //void setTextureSize(int textureSizeWidth, int textureSizeHeight);
    //void setRectSize(int rectSizeWidth, int rectSizeHeight);
    //void setRectPos(std::pair<int, int>* rectPos);
    
    virtual void updatePositions() = 0;
    virtual void updateTexcoords() = 0;
    virtual void updateNormals() = 0;
    
    virtual bool load();
    virtual bool draw();
    virtual void destroy() = 0;
    
};

#endif /* MeshRender_h */
