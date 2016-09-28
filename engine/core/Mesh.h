
#ifndef mesh_h
#define mesh_h

#include "Object.h"
#include "Texture.h"
#include "math/Vector4.h"
#include "render/MeshManager.h"
#include "Submesh.h"

class Mesh: public Object {
protected:
    MeshManager mesh;

    Matrix4 modelViewMatrix;
    Matrix4 normalMatrix;


    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;
    std::vector<Submesh> submeshes;

    int primitiveMode;
public:
    Mesh();
    virtual ~Mesh();

    void setColor(float red, float green, float blue, float alpha);

    void loadTexture(const char* path);
    void setTexture(Texture texture);

    int getPrimitiveMode();
    Vector4 getColor();
    std::vector<Vector3> getVertices();
    std::vector<Vector3> getNormals();
    std::vector<Vector2> getTexcoords();
    std::vector<Submesh> getSubmeshes();

    void setPrimitiveMode(int primitiveMode);
    void addVertex(Vector3 vertex);
    void addNormal(Vector3 normal);
    void addTexcoord(Vector2 texcoord);
    void addSubmesh(Submesh submesh);

    void update();

    bool load();
    bool draw();
    void destroy();

};

#endif /* mesh_h */
