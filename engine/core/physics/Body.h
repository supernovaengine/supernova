#ifndef BODY_H
#define BODY_H

#include "math/Vector3.h"
#include "math/Quaternion.h"
#include <string>

//
// (c) 2018 Eduardo Doria.
//

namespace Supernova {
    class ConcreteObject;

    class Body {

    private:
        std::string name;

    protected:
        bool is3D;
        Vector3 center;
        ConcreteObject* attachedObject;

        Body();

        virtual void computeShape() = 0;

    public:
        virtual ~Body();

        virtual Vector3 getPosition() = 0;
        virtual Quaternion getRotation() = 0;

        void setCenter(Vector3 center);
        void setCenter(const float x, const float y, const float z);
        Vector3 getCenter();

        void setName(std::string name);
        std::string getName();

        virtual void updateObject(ConcreteObject* object);
    };
}


#endif //BODY_H
