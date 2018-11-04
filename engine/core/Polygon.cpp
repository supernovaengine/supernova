#include "Polygon.h"

#include "Log.h"
#include "render/ObjectRender.h"

using namespace Supernova;

Polygon::Polygon(): Mesh2D() {
	primitiveType = S_PRIMITIVE_TRIANGLES;

	vertexBuffer.setName("vertices");
	vertexBuffer.addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    vertexBuffer.addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    vertexBuffer.addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);
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
        updateTexcoords();
    }
}

void Polygon::clear(){
    vertexBuffer.clearBuffer();
}

void Polygon::addVertex(Vector3 vertex){

    vertexBuffer.addValue(S_VERTEXATTRIBUTE_VERTICES, vertex);
    vertexBuffer.addValue(S_VERTEXATTRIBUTE_NORMALS, Vector3(0.0f, 0.0f, 1.0f));

    if (vertexBuffer.getVertexSize() > 3){
        primitiveType = S_PRIMITIVE_TRIANGLES_STRIP;
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

    AttributeData* attVertex = vertexBuffer.getAttribute(S_VERTEXATTRIBUTE_VERTICES);

    for ( unsigned int i = 0; i < vertexBuffer.getVertexSize(); i++){
        min_X = fmin(min_X, vertexBuffer.getValue(attVertex, i, 0));
        min_Y = fmin(min_Y, vertexBuffer.getValue(attVertex, i, 1));
        max_X = fmax(max_X, vertexBuffer.getValue(attVertex, i, 0));
        max_Y = fmax(max_Y, vertexBuffer.getValue(attVertex, i, 1));
    }

    double k_X = 1/(max_X - min_X);
    double k_Y = 1/(max_Y - min_Y);

    float u = 0;
    float v = 0;

    for ( unsigned int i = 0; i < vertexBuffer.getVertexSize(); i++){
        u = (vertexBuffer.getValue(attVertex, i, 0) - min_X) * k_X;
        v = (vertexBuffer.getValue(attVertex, i, 1) - min_Y) * k_Y;
        if (invertTexture) {
            vertexBuffer.addValue(S_VERTEXATTRIBUTE_TEXTURECOORDS, Vector2(u, 1.0 - v));
        }else{
            vertexBuffer.addValue(S_VERTEXATTRIBUTE_TEXTURECOORDS, Vector2(u, v));
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
