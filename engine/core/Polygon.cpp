#include "Polygon.h"

#include "Log.h"
#include "render/ObjectRender.h"

using namespace Supernova;

Polygon::Polygon(): Mesh2D() {
	primitiveType = S_PRIMITIVE_TRIANGLES;
}

Polygon::~Polygon() {
}

void Polygon::setSize(int width, int height){
    Log::Error("Can't set size of Polygon");
}

void Polygon::setInvertTexture(bool invertTexture){
    Mesh2D::setInvertTexture(invertTexture);
    if (loaded) {
        generateTexcoords();
        updateVertices();
    }
}

void Polygon::clear(){
    vertices.clear();
    normals.clear();
    texcoords.clear();
}

void Polygon::addVertex(Vector3 vertex){
	vertices.push_back(vertex);

    if (vertices.size() > 3){
        primitiveType = S_PRIMITIVE_TRIANGLES_STRIP;
    }

    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
}

void Polygon::addVertex(float x, float y){
   addVertex(Vector3(x, y, 0));
}

void Polygon::generateTexcoords(){

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
    texcoords.clear();
    for ( unsigned int i = 0; i < vertices.size(); i++){
        u = (vertices[i].x - min_X) * k_X;
        v = (vertices[i].y - min_Y) * k_Y;
        if (invertTexture) {
            texcoords.push_back(Vector2(u, 1.0 - v));
        }else{
            texcoords.push_back(Vector2(u, v));
        }
    }

    width = (int)(max_X - min_X);
    height = (int)(max_Y - min_Y);
}

bool Polygon::load(){

    setInvertTexture(isIn3DScene());
    generateTexcoords();

    return Mesh::load();

}
