#ifndef submesh_h
#define submesh_h

#include "math/Vector4.h"
#include <string>
#include <vector>

class Submesh {
    friend class Mesh;
    friend class Model;
private:
    
    bool loaded;
    
    bool transparent;

    std::vector<std::string> textures;
    Vector4 color;
    std::vector<unsigned int> indices;
    
    float distanceToCamera;

public:
    Submesh();
    Submesh(const Submesh& s);
    virtual ~Submesh();

    Submesh& operator = (const Submesh& s);

    void setTexture(std::string texture);
    void setColor(Vector4 color);
    void setIndices(std::vector<unsigned int> indices);
    void addIndex(unsigned int index);

    std::vector<std::string> getTextures();
    Vector4* getColor();
    std::vector<unsigned int>* getIndices();
    unsigned int getIndex(int offset);

};

#endif /* Submesh_h */
