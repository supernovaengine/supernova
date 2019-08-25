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

void Lines::setColor(Vector4 color){
    if (color.w != 1){
        transparent = true;
    }
    this->color = color;
}

void Lines::setColor(float red, float green, float blue, float alpha){
    setColor(Vector4(red, green, blue, alpha));
}

Vector4 Lines::getColor(){
    return this->color;
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

bool Lines::renderDraw(bool shadow){
    if (!GraphicObject::renderDraw(shadow))
        return false;

    render->prepareDraw();
    render->draw();
    render->finishDraw();

    return true;
}

bool Lines::load(){

    if (color.w != 1){
        transparent = true;
    }

    instanciateRender();

    render->setPrimitiveType(S_PRIMITIVE_LINES);
    render->setProgramShader(S_SHADER_LINES);

    render->setLineWidth(lineWidth);

    render->addProperty(S_PROPERTY_COLOR, S_PROPERTYDATA_FLOAT4, 1, &color);

    return GraphicObject::load();
}
