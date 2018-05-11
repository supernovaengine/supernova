#ifndef BODY2D_H
#define BODY2D_H

#define S_BODY2D_SHAPE_BOX 0
#define S_BODY2D_SHAPE_VERTICES 1
#define S_BODY2D_SHAPE_CIRCLE 2

#include "Body.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include <vector>

//
// (c) 2018 Eduardo Doria.
//

class b2World;
class b2BodyDef;
class b2Body;
class b2Shape;
class b2FixtureDef;

namespace Supernova {
    class Body2D: public Body {

        friend class PhysicsWorld2D;

    private:
        b2Body* body;
        b2BodyDef* bodyDef;
        b2Shape* shape;
        b2FixtureDef* fixtureDef;

        bool dynamic;
        int shapeType;

        std::vector<Vector2> vertices;
        float boxWidth;
        float boxHeight;
        Vector2 circleCenter;
        float circleRadius;

    protected:
        virtual void computeShape();

    public:
        Body2D();
        virtual ~Body2D();

        void createBody(b2World* world);

        void setDynamic(bool dynamic);
        void setPosition(Vector2 position);
        void setShapeBox(float width, float height);
        void setShapeVertices(std::vector<Vector2> vertices);
        void setShapeVertices(std::vector<Vector3> vertices);
        void setShapeCircle(Vector2 center, float radius);

        void setDensity(float density);
        void setFriction(float friction);
        void setFixedRotation(bool fixedRotation);
        void setLinearVelocity(Vector2 linearVelocity);

        float getDensity();
        float getFriction();
        bool getFixedRotation();
        Vector2 getLinearVelocity();

        void applyForce(const Vector2 force, const Vector2 point);

        virtual Vector3 getPosition();
        virtual Quaternion getRotation();
    };
}

#endif //BODY2D_H
