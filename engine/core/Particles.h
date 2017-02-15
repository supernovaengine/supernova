
#ifndef PARTICLES_H
#define PARTICLES_H

#include "ConcreteObject.h"
#include "render/DrawManager.h"

class Particles: public ConcreteObject {

protected:
    DrawManager renderManager;

    std::vector<Vector3> positions;
    std::vector<Vector3> normals;

public:
    Particles();
    virtual ~Particles();

    bool render();
    bool load();
    bool draw();
};


#endif
