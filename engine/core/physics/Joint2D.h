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

    class Joint2D {
    private:
        b2Joint* joint;
        b2JointDef* jointDef;

        PhysicsWorld2D* world;

    public:
        Joint2D();
        virtual ~Joint2D();

        void createJoint(PhysicsWorld2D* world);
        void destroyJoint();

        void setBodyA(Body2D* body);
        Body2D* getBodyA();

        void setBodyB(Body2D* body);
        Body2D* getBodyB();

        void setCollideConnected(bool collideConnected);
        bool getCollideConnected();
    };
}


#endif //JOINT2D_H
