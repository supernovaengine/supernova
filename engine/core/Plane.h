#ifndef plane_h
#define plane_h

#include <vector>
#include "Mesh.h"
#include "math/Vector3.h"

namespace Supernova {

    class Plane: public Mesh {
    public:
        Plane();
        Plane(float width, float depth);
        virtual ~Plane();

    };
    
}

#endif /* plane_h */
