#ifndef shape_h
#define shape_h

#include <vector>
#include "Mesh2D.h"
#include "math/Vector3.h"

namespace Supernova {

    class Polygon: public Mesh2D {

    private:
        InterleavedBuffer buffer;

    public:
        Polygon();
        virtual ~Polygon();

        void generateTexcoords();

        virtual void setSize(int width, int height);
        virtual void setInvertTexture(bool invertTexture);

        void clear();
        void addVertex(Vector3 vertex);
        void addVertex(float x, float y);
        
        virtual bool load();
    };
    
}

#endif /* shape_h */
