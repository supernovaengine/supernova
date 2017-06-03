
#ifndef mesh_h
#define mesh_h

#include "ConcreteObject.h"
#include "math/Vector4.h"
#include "render/MeshManager.h"
#include "Submesh.h"

class Mesh: public ConcreteObject {
    
private:
    void removeAllSubmeshes();
    
protected:
    MeshManager renderManager;

    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;
    std::vector<Submesh*> submeshes;
    
    bool skymesh;
    bool dynamic;

    int primitiveMode;
    
    void addSubmesh(Submesh* submesh);
    

    void sortTransparentSubmeshes();
    
public:
    Mesh();
    virtual ~Mesh();

    int getPrimitiveMode();
    std::vector<Vector3>* getVertices();
    std::vector<Vector3>* getNormals();
    std::vector<Vector2>* getTexcoords();
    std::vector<Submesh*>* getSubmeshes();
    bool isSky();
    bool isDynamic();
    
    void setTexcoords(std::vector<Vector2> texcoords);

    void setPrimitiveMode(int primitiveMode);
    void addVertex(Vector3 vertex);
    void addNormal(Vector3 normal);
    void addTexcoord(Vector2 texcoord);

    void transform(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
    void update();

    bool render();

    bool load();
    bool draw();
    void destroy();

};

#endif /* mesh_h */
