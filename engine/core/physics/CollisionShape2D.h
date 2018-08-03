#ifndef COLLISIONHAPE2D_H
#define COLLISIONHAPE2D_H

#include "CollisionShape.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include <vector>
#include <string>

#define S_COLLISIONSHAPE2D_BOX 0
#define S_COLLISIONSHAPE2D_VERTICES 1
#define S_COLLISIONSHAPE2D_CIRCLE 2

//
// (c) 2018 Eduardo Doria.
//

class b2Fixture;
class b2FixtureDef;
class b2Shape;

namespace Supernova {

    class Body2D;

    class CollisionShape2D: public CollisionShape {

    private:
        b2Fixture* fixture;
        b2FixtureDef* fixtureDef;
        b2Shape* shape;

        Body2D* body;

        int shapeType;

        std::vector<Vector2> vertices;
        float boxWidth;
        float boxHeight;
        float circleRadius;

        Vector2 center;

        void computeShape(float scale = 1);

    public:
        CollisionShape2D();
        CollisionShape2D(float boxWidth, float boxHeight);
        CollisionShape2D(float boxWidth, float boxHeight, Vector2 center);
        CollisionShape2D(std::vector<Vector2> vertices);
        CollisionShape2D(Vector2 circleCenter, float circleRadius);
        
        virtual ~CollisionShape2D();

        void create(Body2D* body);
        void destroy();

        b2Fixture* getBox2DFixture();
        Body2D* getBody();

        void setShapeBox(float width, float height);
        void setShapeVertices(std::vector<Vector2> vertices);
        void setShapeVertices(std::vector<Vector3> vertices);
        void setShapeCircle(Vector2 center, float radius);

        void setDensity(float density);
        float getDensity();

        void setFriction(float friction);
        float getFriction();

        void setRestituition(int restituition);
        int getRestituition();

        void setSensor(bool sensor);
        bool isSensor();

        void setCenter(Vector2 center);
        void setCenter(const float x, const float y);
        Vector2 getCenter();

    };
}


#endif //COLLISIONHAPE2D_H
