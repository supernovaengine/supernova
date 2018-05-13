#ifndef BODY_H
#define BODY_H

#include "math/Vector3.h"
#include "math/Quaternion.h"
#include <string>
#include <vector>
#include "CollisionShape.h"

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
        std::vector<CollisionShape*> shapes;
        bool ownedShapes;

        Body();

    public:
        virtual ~Body();

        virtual void setPosition(Vector3 position) = 0;
        virtual Vector3 getPosition() = 0;

        virtual void setRotation(Quaternion rotation) = 0;
        virtual Quaternion getRotation() = 0;

        void setName(std::string name);
        std::string getName();

        void setOwnedShapes(bool ownedShapes);
        bool isOwnedShapes();

        void updateObject(ConcreteObject* object);
    };
}


#endif //BODY_H
