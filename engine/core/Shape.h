#ifndef shape_h
#define shape_h

#include <vector>
#include "Mesh.h"
#include "math/Vector3.h"

namespace Supernova {

    class Shape: public Mesh {

    public:
        Shape();
        virtual ~Shape();

        void generateTexcoords();

        void addVertex(Vector3 vertex);
        void addVertex(float x, float y);
        
        virtual bool load();
    };
    
}

#endif /* shape_h */
