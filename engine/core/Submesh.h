#ifndef submesh_h
#define submesh_h

#include "math/Vector4.h"
#include <string>
#include <vector>

class Submesh {
private:
    
    bool loaded;

    std::string texture;
    Vector4 color;
    std::vector<unsigned int> indices;

public:
    Submesh();
    Submesh(const Submesh& s);
    virtual ~Submesh();

    Submesh& operator = (const Submesh& s);

    void setTexture(std::string texture);
    void setColor(Vector4 color);
    void setIndices(std::vector<unsigned int> indices);
    void addIndex(unsigned int index);

    std::string getTexture();
    Vector4* getColor();
    std::vector<unsigned int>* getIndices();

};

#endif /* Submesh_h */
