#include "Shape.h"

#include "platform/Log.h"
#include "PrimitiveMode.h"

Shape::Shape(): Mesh() {
	primitiveMode = S_TRIANGLES;
}

Shape::~Shape() {
}

void Shape::addVertex(Vector3 vertex){
	vertices.push_back(vertex);

    if (vertices.size() > 3){
        primitiveMode = S_TRIANGLES_STRIP;
    }

    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
}

void Shape::addVertex(float x, float y){

   addVertex(Vector3(x, y, 0));

}

void Shape::generateTexcoords(){

    float min_X = std::numeric_limits<float>::max();
    float max_X = std::numeric_limits<float>::min();
    float min_Y = std::numeric_limits<float>::max();
    float max_Y = std::numeric_limits<float>::min();

    for ( unsigned int i = 0; i < vertices.size(); i++){
        min_X = fmin(min_X, vertices[i].x);
        min_Y = fmin(min_Y, vertices[i].y);
        max_X = fmax(max_X, vertices[i].x);
        max_Y = fmax(max_Y, vertices[i].y);
    }

    double k_X = 1/(max_X - min_X);
    double k_Y = 1/(max_Y - min_Y);

    float u = 0;
    float v = 0;
    for ( unsigned int i = 0; i < vertices.size(); i++){
        u = (vertices[i].x - min_X) * k_X;
        v = (vertices[i].y - min_Y) * k_Y;
        texcoords.push_back(Vector2(u, 1.0 - v));
    }

}

bool Shape::load(){

    if (submeshes[0]->getMaterial()->getTextures().size() > 0 && (texcoords.size()==0))
        generateTexcoords();

    return Mesh::load();

}
