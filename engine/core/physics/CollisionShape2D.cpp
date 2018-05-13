#include "CollisionShape2D.h"

#include "Log.h"
#include "Body2D.h"
#include "PhysicsWorld2D.h"
#include <Box2D/Box2D.h>

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

CollisionShape2D::CollisionShape2D(): CollisionShape(){
    shape = NULL;
    fixtureDef = new b2FixtureDef();

    shapeType = S_COLLISIONSHAPE2D_BOX;

    body = NULL;

    boxWidth = 0.0f;
    boxHeight = 0.0f;
    circleCenter = Vector2();
    circleRadius = 0.0f;

    center = Vector2(0.0f, 0.0f);
}

CollisionShape2D::~CollisionShape2D(){
    destroyFixture();

    delete fixtureDef;
    delete shape;
}

void CollisionShape2D::createFixture(Body2D* body){
    fixture = body->getBox2DBody()->CreateFixture(fixtureDef);
    fixture->SetUserData(this);
    this->body = body;
}

void CollisionShape2D::destroyFixture(){
    if (fixture && body) {
        body->getBox2DBody()->DestroyFixture(fixture);
        fixture = NULL;
        body = NULL;
    }
}

b2Fixture* CollisionShape2D::getBox2DFixture(){
    return fixture;
}

Body2D* CollisionShape2D::getBody(){
    return body;
}

void CollisionShape2D::setShapeBox(float width, float height){
    this->boxWidth = width/2.0f;
    this->boxHeight = height/2.0f;

    if (shape)
        delete shape;

    shapeType = S_COLLISIONSHAPE2D_BOX;
    shape = new b2PolygonShape();
    fixtureDef->shape = shape;

    computeShape();
}

void CollisionShape2D::setShapeVertices(std::vector<Vector2> vertices){
    this->vertices = vertices;

    if (shape)
        delete shape;

    shapeType = S_COLLISIONSHAPE2D_VERTICES;
    shape = new b2PolygonShape();
    fixtureDef->shape = shape;

    computeShape();
}

void CollisionShape2D::setShapeCircle(Vector2 center, float radius){
    this->circleCenter = center;
    this->circleRadius = radius;

    if (shape)
        delete shape;

    shapeType = S_COLLISIONSHAPE2D_CIRCLE;
    shape = new b2CircleShape();
    fixtureDef->shape = shape;

    computeShape();
}

void CollisionShape2D::setShapeVertices(std::vector<Vector3> vertices){
    this->vertices.clear();
    for (int i = 0; i < vertices.size(); i++){
        this->vertices.push_back(Vector2(vertices[i].x, vertices[i].y));
    }

    shapeType = S_COLLISIONSHAPE2D_VERTICES;

    computeShape();
}

void CollisionShape2D::computeShape(){

    if (shapeType == S_COLLISIONSHAPE2D_BOX){

        if (boxWidth > 0 && boxHeight > 0) {
            ((b2PolygonShape *) shape)->SetAsBox(boxWidth / S_POINTS_TO_METER_RATIO,
                                                 boxHeight / S_POINTS_TO_METER_RATIO,
                                                 b2Vec2((boxWidth - center.x) / S_POINTS_TO_METER_RATIO, (boxHeight - center.y) / S_POINTS_TO_METER_RATIO),
                                                 0);
        }else{
            Log::Error("Cannot create shape box with size 0");
        }

    }else if (shapeType == S_COLLISIONSHAPE2D_VERTICES) {

        if (b2_maxPolygonVertices >= vertices.size() && vertices.size() > 0) {

            b2Vec2 b2Vertices[b2_maxPolygonVertices];

            b2Transform xf;
            xf.p = b2Vec2(center.x, center.y);
            xf.q.Set(0);

            for (int i = 0; i < vertices.size(); i++){
                b2Vertices[i].Set(vertices[i].x / S_POINTS_TO_METER_RATIO, vertices[i].y / S_POINTS_TO_METER_RATIO);
                b2Vertices[i] = b2Mul(xf, b2Vertices[i]);
            }

            ((b2PolygonShape *) shape)->Set(b2Vertices, (int)vertices.size());

        }else if (vertices.size() == 0){
            Log::Error("Cannot create shape because number of vertices is 0");
        }else{
            Log::Error("Cannot create shape because number of vertices must be less or equal than %i", b2_maxPolygonVertices);
        }

    }else if (shapeType == S_COLLISIONSHAPE2D_CIRCLE) {

        if (circleRadius > 0){
            ((b2CircleShape *) shape)->m_p.Set(circleCenter.x / S_POINTS_TO_METER_RATIO, circleCenter.y / S_POINTS_TO_METER_RATIO);
            ((b2CircleShape *) shape)->m_radius = circleRadius / S_POINTS_TO_METER_RATIO;
        }else{
            Log::Error("Cannot create shape circle with radius 0");
        }

    }
}

void CollisionShape2D::setDensity(float density){
    fixtureDef->density = density;
}

float CollisionShape2D::getDensity(){
    return fixtureDef->density;
}

void CollisionShape2D::setFriction(float friction){
    fixtureDef->friction = friction;
}

float CollisionShape2D::getFriction(){
    return fixtureDef->friction;
}

void CollisionShape2D::setCenter(Vector2 center){
    this->center = center;
    computeShape();
}

void CollisionShape2D::setCenter(const float x, const float y){
    setCenter(Vector2(x, y));
}

Vector2 CollisionShape2D::getCenter(){
    return center;
}