#include "Text.h"

#include "math/Color.h"
#include "util/STBText.h"

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

void Text::setSize(int width, int height){
    TextComponent& textcomp = getComponent<TextComponent>();
    UIRenderComponent& uicomp = getComponent<UIRenderComponent>();

    uicomp.width = width;
    uicomp.height = height;
    textcomp.userDefinedWidth = true;
    textcomp.userDefinedHeight = true;

    textcomp.needUpdateText = true;
}

void Text::setWidth(int width){
    TextComponent& textcomp = getComponent<TextComponent>();
    UIRenderComponent& uicomp = getComponent<UIRenderComponent>();

    uicomp.width = width;
    textcomp.userDefinedWidth = true;

    textcomp.needUpdateText = true;
}

void Text::setHeight(int height){
    TextComponent& textcomp = getComponent<TextComponent>();
    UIRenderComponent& uicomp = getComponent<UIRenderComponent>();

    uicomp.height = height;
    textcomp.userDefinedHeight = true;

    textcomp.needUpdateText = true;
}

void Text::setMaxLength(unsigned int maxLength){
    TextComponent& textcomp = getComponent<TextComponent>();
    UIRenderComponent& uicomp = getComponent<UIRenderComponent>();

    if (textcomp.maxLength != maxLength){
        textcomp.maxLength = maxLength;

        textcomp.needReload = true;
        textcomp.needUpdateText = true;
        uicomp.needReload = true;
    }
}

void Text::setText(std::string text){
    TextComponent& textcomp = getComponent<TextComponent>();

    if (text.length() > textcomp.maxLength){
        text.resize(textcomp.maxLength);
        Log::Warn("Text is bigger than maxLength: %i", textcomp.maxLength);
    }

    textcomp.text = text;
    textcomp.needUpdateText = true;
}

std::string Text::getText(){
    TextComponent& textcomp = getComponent<TextComponent>();

    return textcomp.text;
}

void Text::setFont(std::string font){
    TextComponent& textcomp = getComponent<TextComponent>();

    if (textcomp.font != font){
        textcomp.font = font;

        textcomp.needReload = true;
        textcomp.needUpdateText = true;
    }
}

std::string Text::getFont(){
    TextComponent& textcomp = getComponent<TextComponent>();

    return textcomp.font;
}

void Text::setFontSize(unsigned int fontSize){
    TextComponent& textcomp = getComponent<TextComponent>();

    textcomp.fontSize = fontSize;

    textcomp.needReload = true;
    textcomp.needUpdateText = true;
}

void Text::setMultiline(bool multiline){
    TextComponent& textcomp = getComponent<TextComponent>();

    textcomp.multiline = multiline;
    textcomp.needUpdateText = true;
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

float Text::getAscent(){
    TextComponent& textcomp = getComponent<TextComponent>();

    if (!textcomp.stbtext)
        return 0;
    else
        return textcomp.stbtext->getAscent();
}

float Text::getDescent(){
    TextComponent& textcomp = getComponent<TextComponent>();

    if (!textcomp.stbtext)
        return 0;
    else
        return textcomp.stbtext->getDescent();
}

float Text::getLineGap(){
    TextComponent& textcomp = getComponent<TextComponent>();

    if (!textcomp.stbtext)
        return 0;
    else
        return textcomp.stbtext->getLineGap();
}

int Text::getLineHeight(){
    TextComponent& textcomp = getComponent<TextComponent>();

    if (!textcomp.stbtext)
        return 0;
    else
        return textcomp.stbtext->getLineHeight();
}

unsigned int Text::getNumChars(){
    TextComponent& textcomp = getComponent<TextComponent>();

    return textcomp.charPositions.size();
}

Vector2 Text::getCharPosition(unsigned int index){
    TextComponent& textcomp = getComponent<TextComponent>();

    if (index >= 0 && index < textcomp.charPositions.size()){
        return textcomp.charPositions[index];
    }

    throw std::out_of_range("charPositions out of range");
}