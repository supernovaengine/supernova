#ifndef submesh_h
#define submesh_h

#include "Texture.h"
#include "math/Vector4.h"

class Submesh {
private:

    Texture texture;
    Vector4 color;
    std::vector<unsigned int> indices;

public:
    Submesh();
    Submesh(const Submesh& s);
    virtual ~Submesh();

    Submesh& operator = (const Submesh& s);

    void setTexture(Texture texture);
    void setColor(Vector4 color);
    void setIndices(std::vector<unsigned int> indices);
    void addIndex(unsigned int index);

    Texture* getTexture();
    Vector4* getColor();
    std::vector<unsigned int>* getIndices();

};

#endif /* Submesh_h */
