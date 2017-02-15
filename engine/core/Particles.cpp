#include "Particles.h"

#include "PrimitiveMode.h"
#include "Scene.h"

Particles::Particles(){

    positions.push_back(Vector3(50, 50, 0));
}

Particles::~Particles(){

}

bool Particles::render(){
    return renderManager.draw();
}

bool Particles::load(){

    if (scene != NULL){
        renderManager.getRender()->setSceneRender(((Scene*)scene)->getSceneRender());
    }

    renderManager.getRender()->setPositions(&positions);
    renderManager.getRender()->setNormals(&normals);
    renderManager.getRender()->setMaterial(&material);

    renderManager.getRender()->setObjectType(S_DRAW_POINTS);
    renderManager.getRender()->setPrimitiveMode(S_POINTS);

    renderManager.load();

    return ConcreteObject::load();
}

bool Particles::draw(){
    renderManager.getRender()->setModelMatrix(&modelMatrix);
    renderManager.getRender()->setNormalMatrix(&normalMatrix);
    renderManager.getRender()->setModelViewProjectionMatrix(&modelViewProjectionMatrix);
    renderManager.getRender()->setCameraPosition(cameraPosition);

    return ConcreteObject::draw();
}
