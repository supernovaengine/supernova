#ifndef plane_h
#define plane_h

#include <vector>
#include "Mesh.h"
#include "math/Vector3.h"
#include "render/MeshRender.h"

class Plane: public Mesh {
public:
    Plane();
    Plane(float width, float depth);
    virtual ~Plane();

};

#endif /* plane_h */
