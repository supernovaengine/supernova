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

    void checkLighting();
    void checkFog();
    
protected:

    bool loaded;
    bool lighting;
    bool hasfog;
    
    Mesh* mesh;

    std::unordered_map<Submesh*, SubmeshStruct> submeshesGles;
    
    SceneRender* sceneRender;

    bool isSky;
    //bool isRectImage;

    TextureRect* textureRect;
    
public:
    
    MeshRender();
    virtual ~MeshRender();
    
    void setMesh(Mesh* mesh);
    
    void setSceneRender(SceneRender* sceneRender);

    void setIsSky(bool isSky);
    //void setIsRectImage(bool isRectImage);
    //void setTextureRect(TextureRect* textureRect);
    
    virtual void updatePositions() = 0;
    virtual void updateTexcoords() = 0;
    virtual void updateNormals() = 0;
    
    virtual bool load();
    virtual bool draw();
    virtual void destroy() = 0;
    
};

#endif /* MeshRender_h */
