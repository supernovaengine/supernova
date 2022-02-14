//
// (c) 2022 Eduardo Doria.
//


#include "MeshPolygon.h"

#include "Engine.h"
#include "component/MeshPolygonComponent.h"

using namespace Supernova;

MeshPolygon::MeshPolygon(Scene* scene): Mesh(scene){
    addComponent<MeshPolygonComponent>({});

    MeshComponent& mesh = getComponent<MeshComponent>();
    mesh.submeshes[0].primitiveType = PrimitiveType::TRIANGLE_STRIP;
}

void MeshPolygon::addVertex(Vector3 vertex){
    MeshPolygonComponent& pcomp = getComponent<MeshPolygonComponent>();

    pcomp.points.push_back({vertex, Vector4(1.0f, 1.0f, 1.0f, 1.0f)});

    pcomp.needUpdatePolygon = true;
}

void MeshPolygon::addVertex(float x, float y){
   addVertex(Vector3(x, y, 0));
}

int MeshPolygon::getWidth(){
    MeshPolygonComponent& pcomp = getComponent<MeshPolygonComponent>();

    return pcomp.width;
}

int MeshPolygon::getHeight(){
    MeshPolygonComponent& pcomp = getComponent<MeshPolygonComponent>();

    return pcomp.height;
}

void MeshPolygon::setFlipY(bool flipY){
    MeshPolygonComponent& pcomp = getComponent<MeshPolygonComponent>();

    if (!Engine::isAutomaticFlipY()){
        if (pcomp.flipY != flipY){
            pcomp.flipY = flipY;

            pcomp.needUpdatePolygon = true;
        }
    }else{
        Log::Error("FlipY cannot be changed until disabled automatic by Engine::setAutomaticFlipY()");
    }
}