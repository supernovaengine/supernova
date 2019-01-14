#ifndef cube_h
#define cube_h

#include <vector>
#include "Mesh.h"
#include "math/Vector3.h"

namespace Supernova {

    class Cube: public Mesh {

    private:
        InterleavedBuffer buffer;
        IndexBuffer indices;

        void createVertices();
        void createTexcoords();
        void createIndices();
        void createNormals();

    protected:
        float width;
        float height;
        float depth;

    public:
        Cube();
        Cube(float width, float height, float depth);
        virtual ~Cube();

        virtual bool load();

    };
    
}

#endif /* cube_h */
