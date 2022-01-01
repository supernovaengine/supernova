//
// (c) 2022 Eduardo Doria.
//


#include "Polygon.h"

using namespace Supernova;

Polygon::Polygon(Scene* scene): Mesh(scene){
    MeshComponent& mesh = getComponent<MeshComponent>();
    mesh.submeshes[0].primitiveType = PrimitiveType::TRIANGLE_STRIP;
}

void Polygon::addVertex(Vector3 vertex){

    buffer.addVector3(AttributeType::POSITION, vertex);
    buffer.addVector3(AttributeType::NORMAL, Vector3(0.0f, 0.0f, 1.0f));
    buffer.addVector4(AttributeType::COLOR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    if (buffer.getCount() > 3){
        generateTexcoords();
    }
}

void Polygon::addVertex(float x, float y){
   addVertex(Vector3(x, y, 0));
}

void Polygon::generateTexcoords(){
    float min_X = std::numeric_limits<float>::max();
    float max_X = std::numeric_limits<float>::min();
    float min_Y = std::numeric_limits<float>::max();
    float max_Y = std::numeric_limits<float>::min();

    Attribute* attVertex = buffer.getAttribute(AttributeType::POSITION);

    for ( unsigned int i = 0; i < buffer.getCount(); i++){
        min_X = fmin(min_X, buffer.getFloat(attVertex, i, 0));
        min_Y = fmin(min_Y, buffer.getFloat(attVertex, i, 1));
        max_X = fmax(max_X, buffer.getFloat(attVertex, i, 0));
        max_Y = fmax(max_Y, buffer.getFloat(attVertex, i, 1));
    }

    double k_X = 1/(max_X - min_X);
    double k_Y = 1/(max_Y - min_Y);

    float u = 0;
    float v = 0;

    for ( unsigned int i = 0; i < buffer.getCount(); i++){
        u = (buffer.getFloat(attVertex, i, 0) - min_X) * k_X;
        v = (buffer.getFloat(attVertex, i, 1) - min_Y) * k_Y;
        //if (invertTexture) {
            //buffer.addVector2(AttributeType::TEXTURECOORDS, Vector2(u, 1.0 - v));
       // }else{
            buffer.addVector2(AttributeType::TEXCOORD1, Vector2(u, v));
       // }
    }

    //width = (int)(max_X - min_X);
    //height = (int)(max_Y - min_Y);
}