#ifndef COLLISIONSHAPE_H
#define COLLISIONSHAPE_H

#include <string>
#include "math/Vector3.h"
#include "math/Vector2.h"

//
// (c) 2018 Eduardo Doria.
//

namespace Supernova {
    class CollisionShape {
    private:
        std::string name;

    protected:
        Vector3 position;

        CollisionShape();

    public:
        void setName(std::string name);
        std::string getName();

        void setPosition(Vector3 position);
        void setPosition(Vector2 position);
        void setPosition(float x, float y, float z);
        void setPosition(float x, float y);

    };
}

#endif //COLLISIONSHAPE_H
