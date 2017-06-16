#ifndef cube_h
#define cube_h

#include <vector>
#include "Mesh.h"
#include "math/Vector3.h"

namespace Supernova {

    class Cube: public Mesh {
    public:
        Cube();
        Cube(float width, float height, float depth);
        virtual ~Cube();

    };
    
}

#endif /* cube_h */
