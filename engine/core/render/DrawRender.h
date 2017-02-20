#ifndef DrawRender_h
#define DrawRender_h

#define S_DRAW_MESH 0
#define S_DRAW_SKY 1
#define S_DRAW_POINTS 2

#include "math/Vector2.h"
#include "render/SceneRender.h"
#include "Submesh.h"


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
    
    int primitiveMode;
    int objectType;

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
    void setObjectType(int objectType);
    void setMaterial(Material* material);
    
    virtual void update() = 0;
    virtual bool load() = 0;
    virtual bool draw() = 0;
    virtual void destroy() = 0;
    
};

#endif /* DrawRender_h */
