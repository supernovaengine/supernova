//
// (c) 2022 Eduardo Doria.
//


#include "Polygon.h"

#include "Engine.h"
#include "component/PolygonComponent.h"

using namespace Supernova;

Polygon::Polygon(Scene* scene): Mesh(scene){
    addComponent<PolygonComponent>({});

    MeshComponent& mesh = getComponent<MeshComponent>();
    mesh.submeshes[0].primitiveType = PrimitiveType::TRIANGLE_STRIP;
}

void Polygon::addVertex(Vector3 vertex){
    PolygonComponent& pcomp = getComponent<PolygonComponent>();

    pcomp.points.push_back({vertex, Vector4(1.0f, 1.0f, 1.0f, 1.0f)});

    pcomp.needUpdatePolygon = true;
}

void Polygon::addVertex(float x, float y){
   addVertex(Vector3(x, y, 0));
}

int Polygon::getWidth(){
    PolygonComponent& pcomp = getComponent<PolygonComponent>();

    return pcomp.width;
}

int Polygon::getHeight(){
    PolygonComponent& pcomp = getComponent<PolygonComponent>();

    return pcomp.height;
}

void Polygon::setFlipY(bool flipY){
    PolygonComponent& pcomp = getComponent<PolygonComponent>();

    if (!Engine::isAutomaticFlipY()){
        if (pcomp.flipY != flipY){
            pcomp.flipY = flipY;

            pcomp.needUpdatePolygon = true;
        }
    }else{
        Log::Error("FlipY cannot be changed until disabled automatic by Engine::setAutomaticFlipY()");
    }
}