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
    class Object;
    class PhysicsWorld;

    class Body {

        friend class Object;

    private:
        std::string name;

    protected:
        Vector3 center;
        Object* attachedObject;
        std::vector<CollisionShape*> shapes;
        PhysicsWorld* world;

        bool ownedShapes;

        Body();

    public:
        virtual ~Body();

        virtual void setPosition(Vector3 position) = 0;
        virtual Vector3 getPosition() = 0;

        virtual void setRotation(Quaternion rotation) = 0;
        virtual Quaternion getRotation() = 0;

        bool isWorldSpace();

        PhysicsWorld* getWorld();

        void updateObject();

        void setName(std::string name);
        std::string getName();

        void setOwnedShapes(bool ownedShapes);
        bool isOwnedShapes();

    };
}


#endif //BODY_H
