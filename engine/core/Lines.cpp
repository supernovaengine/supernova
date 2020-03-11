//
// (c) 2018 Eduardo Doria.
//

#include "Lines.h"

using namespace Supernova;

Lines::Lines(): GraphicObject(){
    lineWidth = 1.0;

    buffers["lines"] = &buffer;
    defaultBuffer = "lines";

    buffer.addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
}

Lines:: ~Lines(){

}

void Lines::addLine(Vector3 pointA, Vector3 pointB){
    buffer.addVector3(S_VERTEXATTRIBUTE_VERTICES, pointA);
    buffer.addVector3(S_VERTEXATTRIBUTE_VERTICES, pointB);
}

void Lines::clearLines(){
    buffer.clear();
}

float Lines::getLineWidth() const {
    return lineWidth;
}

void Lines::setLineWidth(float lineWidth) {
    Lines::lineWidth = lineWidth;
    if (render)
        render->setLineWidth(lineWidth);
}

bool Lines::load(){

    instanciateRender();

    render->setPrimitiveType(S_PRIMITIVE_LINES);
    render->setProgramShader(S_SHADER_LINES);

    render->setLineWidth(lineWidth);

    return GraphicObject::load();
}
