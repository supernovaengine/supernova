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
}

void Lines::addLine(Vector3 pointA, Vector3 pointB){
    addLine(pointA, pointB, Vector4(1,1,1,1), Vector4(1,1,1,1));
}

void Lines::addLine(Vector3 pointA, Vector3 pointB, Vector3 color){
    addLine(pointA, pointB, Vector4(color, 1.0));
}

void Lines::addLine(Vector3 pointA, Vector3 pointB, Vector4 color){
    addLine(pointA, pointB, color, color);
}

void Lines::addLine(Vector3 pointA, Vector3 pointB, Vector4 colorA, Vector4 colorB){
    LinesComponent& linescomp = getComponent<LinesComponent>();

    linescomp.lines.push_back({pointA, colorA, pointB, colorB});

    linescomp.needReload = true;
}

LineData Lines::getLine(size_t index) const{
    LinesComponent& linescomp = getComponent<LinesComponent>();

    return linescomp.lines.at(index);
}

void Lines::setLine(size_t index, LineData lineData){
    LinesComponent& linescomp = getComponent<LinesComponent>();

    if (linescomp.lines.at(index). pointA != lineData. pointA || 
        linescomp.lines.at(index). pointB != lineData. pointB ||
        linescomp.lines.at(index). colorA != lineData. colorA ||
        linescomp.lines.at(index). colorB != lineData. colorB){
        linescomp.lines.at(index) = lineData;

        linescomp.needReload = true;
    }
    
}

void Lines::setLinePointA(size_t index, Vector3 pointA){
    LinesComponent& linescomp = getComponent<LinesComponent>();

    if (linescomp.lines.at(index).pointA != pointA){
        linescomp.lines.at(index).pointA = pointA;

        linescomp.needReload = true;
    }
}

void Lines::setLinePointB(size_t index, Vector3 pointB){
    LinesComponent& linescomp = getComponent<LinesComponent>();

    if (linescomp.lines.at(index).pointB != pointB){
        linescomp.lines.at(index).pointB = pointB;
        
        linescomp.needReload = true;
    }
}

void Lines::setLineColorA(size_t index, Vector4 colorA){
    LinesComponent& linescomp = getComponent<LinesComponent>();

    if (linescomp.lines.at(index).colorA != colorA){
        linescomp.lines.at(index).colorA = colorA;
        
        linescomp.needReload = true;
    }
}

void Lines::setLineColorB(size_t index, Vector4 colorB){
    LinesComponent& linescomp = getComponent<LinesComponent>();

    if (linescomp.lines.at(index).colorB != colorB){
        linescomp.lines.at(index).colorB = colorB;
        
        linescomp.needReload = true;
    }
}

void Lines::setLineColor(size_t index, Vector3 color){
    setLineColor(index, Vector4(color, 1.0));
}

void Lines::setLineColor(size_t index, Vector4 color){
    LinesComponent& linescomp = getComponent<LinesComponent>();

    if (linescomp.lines.at(index).colorA != color || linescomp.lines.at(index).colorB != color){
        linescomp.lines.at(index).colorA = color;
        linescomp.lines.at(index).colorB = color;

        linescomp.needReload = true;
    }
}

void Lines::clearLines(){
    LinesComponent& linescomp = getComponent<LinesComponent>();

    if (linescomp.lines.size() > 0){
        linescomp.lines.clear();

        linescomp.needReload = true;
    }
}