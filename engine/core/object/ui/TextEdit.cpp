#include "TextEdit.h"

#include "component/TextComponent.h"
#include "subsystem/UISystem.h"

using namespace Supernova;

TextEdit::TextEdit(Scene* scene): Image(scene){
    addComponent<TextEditComponent>({});

    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    scene->getSystem<UISystem>()->createTextEditObjects(entity, tecomp);

    UIComponent& uitext = scene->getComponent<UIComponent>(tecomp.text);
    uitext.color = Vector4(0.0, 0.0, 0.0, 1.0);
}

Text TextEdit::getTextObject() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();

    return Text(scene, tecomp.text);
}

void TextEdit::setDisabled(bool disabled){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();

    tecomp.disabled = disabled;

    tecomp.needUpdateTextEdit = true;
}

bool TextEdit::getDisabled() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();

    return  tecomp.disabled;
}

void TextEdit::setText(std::string text){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(tecomp.text);

    if (textcomp.text != text){
        textcomp.text = text;
        textcomp.needUpdateText = true;
    }
}

std::string TextEdit::getText() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(tecomp.text);

    return textcomp.text;
}

void TextEdit::setTextColor(Vector4 color){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    UIComponent& uitext = scene->getComponent<UIComponent>(tecomp.text);

    uitext.color = color;
}

void TextEdit::setTextColor(const float red, const float green, const float blue, const float alpha){
    setTextColor(Vector4(red, green, blue, alpha));
}

Vector4 TextEdit::getTextColor() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    UIComponent& uitext = scene->getComponent<UIComponent>(tecomp.text);

    return uitext.color;
}

void TextEdit::setTextFont(std::string font){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();

    getTextObject().setFont(font);

    tecomp.needUpdateTextEdit = true;
}

std::string TextEdit::getTextFont() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(tecomp.text);

    return textcomp.font;
}

void TextEdit::setFontSize(unsigned int fontSize){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();

    getTextObject().setFontSize(fontSize);

    tecomp.needUpdateTextEdit = true;
}

unsigned int TextEdit::getFontSize() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();

    return getTextObject().getFontSize();
}

void TextEdit::setMaxTextSize(unsigned int maxTextSize){
    getTextObject().setMaxTextSize(maxTextSize);
}

unsigned int TextEdit::getMaxTextSize() const{
    return getTextObject().getMaxTextSize();
}