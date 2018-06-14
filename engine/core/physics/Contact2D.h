#ifndef CONTACT2D_H
#define CONTACT2D_H

//
// (c) 2018 Eduardo Doria.
//

#include "CollisionShape2D.h"

class b2Contact;
class b2WorldManifold;

namespace Supernova {
    class Contact2D {
    private:
        b2Contact* contact;
        b2WorldManifold* worldManifold;
    public:
        Contact2D(b2Contact* contact);
        Contact2D(const Contact2D& c);
        Contact2D& operator = (const Contact2D& c);
        virtual ~Contact2D();

        CollisionShape2D* getCollisionShapeA();
        CollisionShape2D* getCollisionShapeB();

        Vector2 getNormal();
        Vector2 getPoint(int index);
        int getNumPoints();

        bool isTouching();

        bool isEnabled();
        void setEnabled(bool enabled);

        float getFriction();
        void setFriction(float friction);

        float getRestitution();
        void setRestitution(float restitution);

        float getTangentSpeed();
        void setTangentSpeed(float tangetSpeed);
    };
}


#endif //CONTACT2D_H
