//
// (c) 2023 Eduardo Doria.
//

#ifndef CONTACT2D_H
#define CONTACT2D_H

#include "Entity.h"
#include "Body2D.h"

class b2Contact;

namespace Supernova{

    class Contact2D{
    private:
        Scene* scene;
        b2Contact* contact;

    public:
        Contact2D(Scene* scene, b2Contact* contact);
        virtual ~Contact2D();

        Contact2D(const Contact2D& rhs);
        Contact2D& operator=(const Contact2D& rhs);

        bool isTouching();

        Entity getBodyA();
        Body2D getBodyObjectA();
        size_t getFixtureIndexA();

        Entity getBodyB();
        Body2D getBodyObjectB();
        size_t getFixtureIndexB();
    };
}

#endif //CONTACT2D_H