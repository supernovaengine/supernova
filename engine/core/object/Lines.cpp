//
// (c) 2024 Eduardo Doria.
//

#include "Lines.h"
#include "subsystem/RenderSystem.h"

using namespace Supernova;

Lines::Lines(Scene* scene): Object(scene){
    addComponent<LinesComponent>({});
}

Lines::~Lines(){

}

bool Lines::load(){
    LinesComponent& linescomp = getComponent<LinesComponent>();

    return scene->getSystem<RenderSystem>()->loadLines(entity, linescomp);

    return true;
}

void Lines::addLine(Vector3 pointA, Vector3 pointB){
    LinesComponent& linescomp = getComponent<LinesComponent>();

    linescomp.lines.push_back({pointA, pointB});
}