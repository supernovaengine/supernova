#ifndef PLANETERRAIN_H
#define PLANETERRAIN_H

#include "Mesh.h"

namespace Supernova{
    class PlaneTerrain: public Mesh{

    public:
        PlaneTerrain(Scene* scene);
        PlaneTerrain(Scene* scene, float width, float depth);

        void create(float width, float depth);

    };
}

#endif //PLANETERRAIN_H