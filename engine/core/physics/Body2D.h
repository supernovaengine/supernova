#ifndef BODY2D_H
#define BODY2D_H

#include "Body.h"
#include "math/Vector2.h"

//
// (c) 2018 Eduardo Doria.
//

class b2BodyDef;
class b2Body;
class b2PolygonShape;
class b2FixtureDef;

namespace Supernova {
    class Body2D: public Body {

        friend class PhysicsWorld2D;

    private:
        b2Body* body;
        b2BodyDef* bodyDef;
        b2PolygonShape* polygonShape;
        b2FixtureDef* fixtureDef;

        bool dynamic;

        float boxWidth;
        float boxHeight;

    protected:
        virtual void computeShape();

    public:
        Body2D();

        void setDynamic(bool dynamic);
        void setPosition(Vector2 position);
        void setAsBox(float width, float height);

        void setDensity(float density);
        void setFriction(float friction);

        float getDensity();
        float getFriction();

        virtual Vector3 getPosition();
        virtual Quaternion getRotation();
    };
}

#endif //BODY2D_H
