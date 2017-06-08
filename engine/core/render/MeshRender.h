#ifndef MeshRender_h
#define MeshRender_h

#include "math/Vector2.h"
#include "render/SceneRender.h"
#include "render/TextureRender.h"
#include "Submesh.h"
#include <vector>
#include <unordered_map>
#include "math/Rect.h"


class Mesh;


class MeshRender {

    typedef struct {
        std::shared_ptr<TextureRender> texture;
        unsigned int indicesSizes;
        bool textured;
    } SubmeshStruct;

private:
    
    Mesh* mesh;

    void checkLighting();
    void checkFog();
    void checkTextureRect();
    void fillMeshProperties();
    
protected:
    
    bool lighting;
    bool hasfog;
    bool hasTextureRect;

    std::unordered_map<Submesh*, SubmeshStruct> submeshesRender;
    
    //-------begin mesh properties-------
    SceneRender* sceneRender;
    std::vector<Vector3>* vertices;
    std::vector<Vector2>* texcoords;
    std::vector<Vector3>* normals;
    std::vector<Submesh*>* submeshes;
    
    Matrix4 modelViewProjectMatrix;
    Matrix4 modelMatrix;
    Matrix4 normalMatrix;
    Vector3 cameraPosition;
    
    bool isLoaded;
    bool isSky;
    bool isDynamic;
    
    int primitiveMode;
    //------------end------------
    
public:
    
    MeshRender();
    virtual ~MeshRender();
    
    void setMesh(Mesh* mesh);
    
    virtual void updateVertices();
    virtual void updateTexcoords();
    virtual void updateNormals();
    virtual void updateIndices();
    
    virtual bool load();
    virtual bool draw();
    virtual void destroy();
    
};

#endif /* MeshRender_h */
