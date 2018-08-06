//
// (c) 2018 Eduardo Doria.
//

#ifndef JOINT2D_H
#define JOINT2D_H

#include "Body2D.h"

class b2Joint;
class b2JointDef;

namespace Supernova {

    class PhysicsWorld2D;

    enum Joint2DType
    {
        REVOLUTE,
        PRISMATIC
    };

    class Joint2D {

    private:
        b2Joint* joint;
        b2JointDef* jointDef;

        PhysicsWorld2D* world;

        Joint2DType type;

        bool collideConnected;

        Body2D* bodyA;
        Body2D* bodyB;

        Vector2 localAnchorA;
        Vector2 localAnchorB;

    public:
        Joint2D();
        Joint2D(Joint2DType type);
        virtual ~Joint2D();

        void create(PhysicsWorld2D* world);
        void destroy();

        void setType(Joint2DType type);

        void setBodyA(Body2D* body);
        Body2D* getBodyA();

        void setBodyB(Body2D* body);
        Body2D* getBodyB();

        const Vector2 &getLocalAnchorA() const;
        void setLocalAnchorA(const Vector2 &localAnchorA);

        const Vector2 &getLocalAnchorB() const;
        void setLocalAnchorB(const Vector2 &localAnchorB);

        void setCollideConnected(bool collideConnected);
        bool getCollideConnected();
    };
}


#endif //JOINT2D_H
