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
    uicomp.isFont = true;

	buffer.clearAll();
	buffer.addAttribute(AttributeType::POSITION, 3);
	buffer.addAttribute(AttributeType::TEXCOORD1, 2);

    TextComponent& textcomp = getComponent<TextComponent>();
    textcomp.text = "";

    setText("Edu");
}

Text::~Text() {
}

void Text::setText(std::string text){
    UIRenderComponent& uicomp = getComponent<UIRenderComponent>();
    TextComponent& textcomp = getComponent<TextComponent>();

    textcomp.text = text;
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