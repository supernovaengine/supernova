#include "Text.h"

#include "math/Color.h"

using namespace Supernova;

Text::Text(Scene* scene): Object(scene){
    addComponent<UIRenderComponent>({});
    addComponent<TextComponent>({});

    UIRenderComponent& uicomp = getComponent<UIRenderComponent>();
    uicomp.buffer = &buffer;
    uicomp.indices = &indices;
    uicomp.primitiveType = PrimitiveType::TRIANGLES;

	buffer.clearAll();
	buffer.addAttribute(AttributeType::POSITION, 3);
	buffer.addAttribute(AttributeType::TEXCOORD1, 2);
    buffer.setUsage(BufferUsage::DYNAMIC);

    indices.setUsage(BufferUsage::DYNAMIC);

    TextComponent& textcomp = getComponent<TextComponent>();
    textcomp.text = "";

    uicomp.minBufferCount = textcomp.maxLength * 4;
    uicomp.minIndicesCount = textcomp.maxLength * 6;
}

Text::~Text() {
}

void Text::setText(std::string text){
    TextComponent& textcomp = getComponent<TextComponent>();

    textcomp.text = text;
    textcomp.needUpdate = true;
}

void Text::setColor(Vector4 color){
    UIRenderComponent& uicomp = getComponent<UIRenderComponent>();

    uicomp.color = Color::sRGBToLinear(color);
}

void Text::setColor(float red, float green, float blue, float alpha){
    setColor(Vector4(red, green, blue, alpha));
}

Vector4 Text::getColor(){
    UIRenderComponent& uicomp = getComponent<UIRenderComponent>();

    return Color::linearTosRGB(uicomp.color);
}