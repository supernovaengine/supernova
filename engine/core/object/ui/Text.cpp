#include "Text.h"

#include "util/Color.h"
#include "util/STBText.h"

using namespace Supernova;

Text::Text(Scene* scene): Object(scene){
    addComponent<UIComponent>({});
    addComponent<TextComponent>({});
}

Text::Text(Scene* scene, Entity entity): Object(scene, entity){
}

Text::~Text() {

}

void Text::setSize(int width, int height){
    TextComponent& textcomp = getComponent<TextComponent>();
    UIComponent& uicomp = getComponent<UIComponent>();

    uicomp.width = width;
    uicomp.height = height;
    textcomp.userDefinedWidth = true;
    textcomp.userDefinedHeight = true;

    textcomp.needUpdateText = true;
}

void Text::setWidth(int width){
    TextComponent& textcomp = getComponent<TextComponent>();
    UIComponent& uicomp = getComponent<UIComponent>();

    uicomp.width = width;
    textcomp.userDefinedWidth = true;

    textcomp.needUpdateText = true;
}

void Text::setHeight(int height){
    TextComponent& textcomp = getComponent<TextComponent>();
    UIComponent& uicomp = getComponent<UIComponent>();

    uicomp.height = height;
    textcomp.userDefinedHeight = true;

    textcomp.needUpdateText = true;
}

int Text::getWidth() const{
    UIComponent& uicomp = getComponent<UIComponent>();

    return uicomp.width;
}

int Text::getHeight() const{
    UIComponent& uicomp = getComponent<UIComponent>();

    return uicomp.height;
}

void Text::setMaxTextSize(unsigned int maxTextSize){
    TextComponent& textcomp = getComponent<TextComponent>();
    UIComponent& uicomp = getComponent<UIComponent>();

    if (textcomp.maxTextSize != maxTextSize){
        textcomp.maxTextSize = maxTextSize;

        textcomp.needReload = true;
        textcomp.needUpdateText = true;
        uicomp.needReload = true;
    }
}

unsigned int Text::getMaxTextSize() const{
    TextComponent& textcomp = getComponent<TextComponent>();

    return textcomp.maxTextSize;
}

void Text::setText(std::string text){
    TextComponent& textcomp = getComponent<TextComponent>();

    if (text.length() > textcomp.maxTextSize){
        text.resize(textcomp.maxTextSize);
        Log::warn("Text is bigger than maxTextSize: %i", textcomp.maxTextSize);
    }

    textcomp.text = text;
    textcomp.needUpdateText = true;
}

std::string Text::getText() const{
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

std::string Text::getFont() const{
    TextComponent& textcomp = getComponent<TextComponent>();

    return textcomp.font;
}

void Text::setFontSize(unsigned int fontSize){
    TextComponent& textcomp = getComponent<TextComponent>();

    textcomp.fontSize = fontSize;

    textcomp.needReload = true;
    textcomp.needUpdateText = true;
}

unsigned int Text::getFontSize() const{
    TextComponent& textcomp = getComponent<TextComponent>();

    return textcomp.fontSize;
}

void Text::setMultiline(bool multiline){
    TextComponent& textcomp = getComponent<TextComponent>();

    textcomp.multiline = multiline;
    textcomp.needUpdateText = true;
}

bool Text::getMultiline() const{
    TextComponent& textcomp = getComponent<TextComponent>();

    return textcomp.multiline;
}

void Text::setColor(Vector4 color){
    UIComponent& uicomp = getComponent<UIComponent>();

    uicomp.color = Color::sRGBToLinear(color);
}

void Text::setColor(float red, float green, float blue, float alpha){
    setColor(Vector4(red, green, blue, alpha));
}

Vector4 Text::getColor() const{
    UIComponent& uicomp = getComponent<UIComponent>();

    return Color::linearTosRGB(uicomp.color);
}

float Text::getAscent() const{
    TextComponent& textcomp = getComponent<TextComponent>();

    if (!textcomp.stbtext)
        return 0;
    else
        return textcomp.stbtext->getAscent();
}

float Text::getDescent() const{
    TextComponent& textcomp = getComponent<TextComponent>();

    if (!textcomp.stbtext)
        return 0;
    else
        return textcomp.stbtext->getDescent();
}

float Text::getLineGap() const{
    TextComponent& textcomp = getComponent<TextComponent>();

    if (!textcomp.stbtext)
        return 0;
    else
        return textcomp.stbtext->getLineGap();
}

int Text::getLineHeight() const{
    TextComponent& textcomp = getComponent<TextComponent>();

    if (!textcomp.stbtext)
        return 0;
    else
        return textcomp.stbtext->getLineHeight();
}

unsigned int Text::getNumChars() const{
    TextComponent& textcomp = getComponent<TextComponent>();

    return textcomp.charPositions.size();
}

Vector2 Text::getCharPosition(unsigned int index) const{
    TextComponent& textcomp = getComponent<TextComponent>();

    if (index >= 0 && index < textcomp.charPositions.size()){
        return textcomp.charPositions[index];
    }

    throw std::out_of_range("charPositions out of range");
}