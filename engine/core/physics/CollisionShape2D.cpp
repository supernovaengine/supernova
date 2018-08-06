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
    fixture = NULL;
    shape = NULL;
    fixtureDef = new b2FixtureDef();

    shapeType = S_COLLISIONSHAPE2D_BOX;

    body = NULL;

    boxWidth = 0.0f;
    boxHeight = 0.0f;
    circleRadius = 0.0f;

    center = Vector2(0.0f, 0.0f);
}

CollisionShape2D::CollisionShape2D(float boxWidth, float boxHeight): CollisionShape2D(){
    setShapeBox(boxWidth, boxHeight);
}

CollisionShape2D::CollisionShape2D(float boxWidth, float boxHeight, Vector2 center): CollisionShape2D(){
    setShapeBox(boxWidth, boxHeight);
    setCenter(center);
}

CollisionShape2D::CollisionShape2D(std::vector<Vector2> vertices): CollisionShape2D(){
    setShapeVertices(vertices);
}

CollisionShape2D::CollisionShape2D(Vector2 circleCenter, float circleRadius): CollisionShape2D(){
    setShapeCircle(circleCenter, circleRadius);
}

CollisionShape2D::~CollisionShape2D(){
    destroy();

    delete fixtureDef;
    delete shape;
}

void CollisionShape2D::create(Body2D* body){
    if (!fixture) {
        computeShape(body->getWorld()->getPointsToMeterScale());

        fixture = body->getBox2DBody()->CreateFixture(fixtureDef);
        fixture->SetUserData(this);
        this->body = body;
    }
}

void CollisionShape2D::destroy(){
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

    if (fixture)
        Log::Error("Can not edit the Shape after it is added to a Body2D");
}

void CollisionShape2D::setShapeVertices(std::vector<Vector2> vertices){
    this->vertices = vertices;
    
    if (shape)
        delete shape;
    
    shapeType = S_COLLISIONSHAPE2D_VERTICES;
    shape = new b2PolygonShape();
    fixtureDef->shape = shape;
    
    if (fixture)
        Log::Error("Can not edit the Shape after it is added to a Body2D");
}

void CollisionShape2D::setShapeVertices(std::vector<Vector3> vertices){
    this->vertices.clear();
    for (int i = 0; i < vertices.size(); i++){
        this->vertices.push_back(Vector2(vertices[i].x, vertices[i].y));
    }

    if (shape)
        delete shape;
    
    shapeType = S_COLLISIONSHAPE2D_VERTICES;
    shape = new b2PolygonShape();
    fixtureDef->shape = shape;

    if (fixture)
        Log::Error("Can not edit the Shape after it is added to a Body2D");
}

void CollisionShape2D::setShapeCircle(Vector2 center, float radius){
    this->center = center;
    this->circleRadius = radius;
    
    if (shape)
        delete shape;
    
    shapeType = S_COLLISIONSHAPE2D_CIRCLE;
    shape = new b2CircleShape();
    fixtureDef->shape = shape;
    
    if (fixture)
        Log::Error("Can not edit the Shape after it is added to a Body2D");
}

void CollisionShape2D::computeShape(float scale){

    if (shapeType == S_COLLISIONSHAPE2D_BOX){

        if (boxWidth > 0 && boxHeight > 0) {
            ((b2PolygonShape *) shape)->SetAsBox(boxWidth / scale,
                                                 boxHeight / scale,
                                                 b2Vec2((boxWidth - center.x + position.x) / scale, (boxHeight - center.y + position.y) / scale),
                                                 0);
        }else{
            Log::Error("Can not create shape box with size 0");
        }

    }else if (shapeType == S_COLLISIONSHAPE2D_VERTICES) {

        if (b2_maxPolygonVertices >= vertices.size() && vertices.size() > 0) {

            b2Vec2 b2Vertices[b2_maxPolygonVertices];

            b2Transform xf;
            xf.p = b2Vec2((-center.x + position.x) / scale, (-center.y + position.y) / scale);
            xf.q.Set(0);

            for (int i = 0; i < vertices.size(); i++){
                b2Vertices[i].Set(vertices[i].x / scale, vertices[i].y / scale);
                b2Vertices[i] = b2Mul(xf, b2Vertices[i]);
            }

            ((b2PolygonShape *) shape)->Set(b2Vertices, (int)vertices.size());

        }else if (vertices.size() == 0){
            Log::Error("Can not create shape because number of vertices is 0");
        }else{
            Log::Error("Can not create shape because number of vertices must be less or equal than %i", b2_maxPolygonVertices);
        }

    }else if (shapeType == S_COLLISIONSHAPE2D_CIRCLE) {

        if (circleRadius > 0){
            ((b2CircleShape *) shape)->m_p.Set((center.x + position.x) / scale, (center.y + position.y) / scale);
            ((b2CircleShape *) shape)->m_radius = circleRadius / scale;
        }else{
            Log::Error("Can not create shape circle with radius 0");
        }

    }
}

void CollisionShape2D::setDensity(float density){
    fixtureDef->density = density;
    if (fixture)
        fixture->SetDensity(density);
}

float CollisionShape2D::getDensity(){
    int density = fixtureDef->density;
    if (fixture)
        density = fixture->GetDensity();

    return density;
}

void CollisionShape2D::setFriction(float friction){
    fixtureDef->friction = friction;
    if (fixture)
        fixture->SetFriction(friction);
}

float CollisionShape2D::getFriction(){
    int friction = fixtureDef->friction;
    if (fixture)
        friction = fixture->GetFriction();

    return friction;
}

void CollisionShape2D::setRestituition(int restituition){
    fixtureDef->restitution = restituition;
    if (fixture)
        fixture->SetRestitution(restituition);
}

int CollisionShape2D::getRestituition(){
    int restituition = fixtureDef->restitution;
    if (fixture)
        restituition = fixture->GetRestitution();

    return restituition;
}

void CollisionShape2D::setSensor(bool sensor){
    fixtureDef->isSensor = sensor;
    if (fixture)
        fixture->SetSensor(sensor);
}

bool CollisionShape2D::isSensor(){
    bool sensor = fixtureDef->isSensor;
    if (fixture)
        sensor = fixture->IsSensor();

    return sensor;
}

void CollisionShape2D::setCenter(Vector2 center){
    this->center = center;
    
    if (fixture)
        Log::Error("Can not edit center after it is added to a Body2D");
}

void CollisionShape2D::setCenter(const float x, const float y){
    setCenter(Vector2(x, y));
}

Vector2 CollisionShape2D::getCenter(){
    return center;
}
