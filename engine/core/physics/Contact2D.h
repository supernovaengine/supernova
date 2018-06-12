#ifndef CONTACT2D_H
#define CONTACT2D_H

//
// (c) 2018 Eduardo Doria.
//

#include "CollisionShape2D.h"

class b2Contact;

namespace Supernova {
    class Contact2D {
    private:
        b2Contact* contact;
    public:
        Contact2D(b2Contact* contact);
        Contact2D(const Contact2D& c);
        Contact2D& operator = (const Contact2D& c);
        virtual ~Contact2D();

        CollisionShape2D* getCollisionShapeA();
        CollisionShape2D* getCollisionShapeB();

    };
}


#endif //CONTACT2D_H
