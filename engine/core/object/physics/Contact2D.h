//
// (c) 2023 Eduardo Doria.
//

#ifndef CONTACT2D_H
#define CONTACT2D_H

#include "Entity.h"
#include "Body2D.h"
#include "Manifold2D.h"

class b2Contact;
class b2Manifold;

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

        b2Contact* getBox2DContact();

        Manifold2D getManifold();

        bool isTouching() const;

        Entity getBodyA();
        Body2D getBodyObjectA();
        size_t getFixtureIndexA();

        Entity getBodyB();
        Body2D getBodyObjectB();
        size_t getFixtureIndexB();

        bool isEnabled() const;
        void setEnabled(bool enabled);

        float getFriction() const;
        void setFriction(float friction);

        void resetFriction();

        float getRestitution() const;
        void setRestitution(float restitution);

        void resetRestitution();

        float getTangentSpeed() const;
        void setTangentSpeed(float tangentSpeed);
    };
}

#endif //CONTACT2D_H