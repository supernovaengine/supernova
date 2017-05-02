#ifndef MeshRender_h
#define MeshRender_h

#include "math/Vector2.h"
#include "render/SceneRender.h"
#include "render/TextureRender.h"
#include "Submesh.h"
#include <vector>
#include <unordered_map>
#include "image/TextureRect.h"


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

    bool loaded;
    bool lighting;
    bool hasfog;
    bool hasTextureRect;

    std::unordered_map<Submesh*, SubmeshStruct> submeshesRender;
    
    SceneRender* sceneRender;
    std::vector<Vector3>* vertices;
    std::vector<Vector2>* texcoords;
    std::vector<Vector3>* normals;
    std::vector<Submesh*>* submeshes;
    
    Matrix4 modelViewProjectMatrix;
    Matrix4 modelMatrix;
    Matrix4 normalMatrix;
    Vector3 cameraPosition;
    
    bool isSky;
    int primitiveMode;
    
public:
    
    MeshRender();
    virtual ~MeshRender();
    
    void setMesh(Mesh* mesh);
    
    virtual void updateVertices() = 0;
    virtual void updateTexcoords() = 0;
    virtual void updateNormals() = 0;
    
    virtual bool load();
    virtual bool draw();
    virtual void destroy() = 0;
    
};

#endif /* MeshRender_h */
