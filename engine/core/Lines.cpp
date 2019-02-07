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

bool Lines::renderDraw(){
    if (!GraphicObject::renderDraw())
        return false;

    render->prepareDraw();
    render->draw();
    render->finishDraw();

    return true;
}

bool Lines::load(){

    if (render == NULL)
        render = ObjectRender::newInstance();

    render->setPrimitiveType(S_PRIMITIVE_LINES);
    render->setProgramShader(S_SHADER_LINES);

    render->setLineWidth(lineWidth);

    render->addProperty(S_PROPERTY_COLOR, S_PROPERTYDATA_FLOAT4, 1, material.getColor());

    prepareRender();

    bool renderloaded = render->load();

    if (!GraphicObject::load())
        return false;

    return renderloaded;
}
