#ifndef submesh_h
#define submesh_h

#include "math/Vector4.h"
#include <string>
#include <vector>
#include "Material.h"

class Submesh {
    friend class Mesh;
    friend class Model;
private:
    
    bool loaded;

    std::vector<unsigned int> indices;
    
    Material* material;
    bool newMaterial;
    
    float distanceToCamera;

public:
    Submesh();
    Submesh(Material* material);
    Submesh(const Submesh& s);
    virtual ~Submesh();

    Submesh& operator = (const Submesh& s);

    void setIndices(std::vector<unsigned int> indices);
    void addIndex(unsigned int index);

    std::vector<unsigned int>* getIndices();
    unsigned int getIndex(int offset);

    void createNewMaterial();
    void setMaterial(Material* material);
    Material* getMaterial();

};

#endif /* Submesh_h */
