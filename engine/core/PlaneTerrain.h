#ifndef planeterrain_h
#define planeterrain_h

#include <vector>
#include "Mesh.h"
#include "math/Vector3.h"

namespace Supernova {

    class PlaneTerrain: public Mesh {
    public:
        PlaneTerrain();
        PlaneTerrain(float width, float depth);
        virtual ~PlaneTerrain();

    };
    
}

#endif /* planeterrain_h */
