#ifndef BODY2D_H
#define BODY2D_H

#define S_BODY2D_STATIC 0
#define S_BODY2D_DYNAMIC 1
#define S_BODY2D_KINEMATIC 2

#include "Body.h"
#include "CollisionShape2D.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include <vector>

//
// (c) 2018 Eduardo Doria.
//

class b2BodyDef;
class b2Body;

namespace Supernova {

    class PhysicsWorld2D;

    class Body2D: public Body {

    private:
        b2Body* body;
        b2BodyDef* bodyDef;

        float density;
        float friction;
        int restituition;

    public:
        Body2D();
        virtual ~Body2D();

        void create(PhysicsWorld2D* world);
        void destroy();

        b2Body* getBox2DBody();
        PhysicsWorld2D* getWorld();

        void createBox(float boxWidth, float boxHeight, Vector2 center = Vector2(0.0, 0.0));
        void createCircle(Vector2 center, float radius);

        //-------For all shapes
        void setDensity(float density);
        void setFriction(float friction);
        void setRestituition(int restituition);
        //-------

        void addCollisionShape(CollisionShape2D* shape);
        void removeCollisionShape(CollisionShape2D* shape);

        void setType(int type);
        int getType();

        void setFixedRotation(bool fixedRotation);
        bool getFixedRotation();

        void setLinearVelocity(Vector2 linearVelocity);
        Vector2 getLinearVelocity();

        void setAngularVelocity(float angularVelocity);
        float getAngularVelocity();

        float getMass();

        void applyForce(const Vector2 force, const Vector2 point);
        void applyForceToCenter(const Vector2 force);
        void applyAngularImpulse(const float impulse);
        void applyLinearImpulse(const Vector2 impulse, const Vector2 point);
        void applyLinearImpulseToCenter(const Vector2 impulse);
        void applyTorque(const float torque);

        void setPosition(Vector2 position);
        virtual void setPosition(Vector3 position);
        virtual Vector3 getPosition();

        void setRotation(float angle);
        virtual void setRotation(Quaternion rotation);
        virtual Quaternion getRotation();
    };
}

#endif //BODY2D_H
