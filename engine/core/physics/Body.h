#ifndef BODY_H
#define BODY_H

#include "math/Vector3.h"
#include "math/Quaternion.h"

//
// (c) 2018 Eduardo Doria.
//

namespace Supernova {
    class ConcreteObject;

    class Body {

    protected:
        bool is3D;
        Vector3 center;

        Body();

        virtual void computeShape() = 0;

    public:
        virtual Vector3 getPosition() = 0;
        virtual Quaternion getRotation() = 0;

        void setCenter(Vector3 center);
        void setCenter(const float x, const float y, const float z);
        Vector3 getCenter();

        virtual void updateObject(ConcreteObject* object);
    };
}


#endif //BODY_H
