#include "Button.h"

#include "component/TextComponent.h"
#include "subsystem/UISystem.h"

using namespace Supernova;

Button::Button(Scene* scene): Image(scene){
    addComponent<ButtonComponent>({});

    ButtonComponent& btcomp = getComponent<ButtonComponent>();
    scene->getSystem<UISystem>()->createButtonObjects(entity, btcomp);
}

Text Button::getLabelObject() const{
    ButtonComponent& btcomp = getComponent<ButtonComponent>();

    return Text(scene, btcomp.label);
}

void Button::setLabel(std::string text){
    ButtonComponent& btcomp = getComponent<ButtonComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(btcomp.label);

    if (textcomp.text != text){
        textcomp.text = text;
        textcomp.needUpdateText = true;
    }
}

std::string Button::getLabel() const{
    ButtonComponent& btcomp = getComponent<ButtonComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(btcomp.label);

    return textcomp.text;
}

void Button::setLabelColor(Vector4 color){
    ButtonComponent& btcomp = getComponent<ButtonComponent>();
    UIComponent& uilabel = scene->getComponent<UIComponent>(btcomp.label);

    uilabel.color = color;
}

void Button::setLabelColor(const float red, const float green, const float blue, const float alpha){
    setLabelColor(Vector4(red, green, blue, alpha));
}

Vector4 Button::getLabelColor() const{
    ButtonComponent& btcomp = getComponent<ButtonComponent>();
    UIComponent& uilabel = scene->getComponent<UIComponent>(btcomp.label);

    return uilabel.color;
}

void Button::setLabelFont(std::string font){
    ButtonComponent& btcomp = getComponent<ButtonComponent>();

    getLabelObject().setFont(font);

    btcomp.needUpdateButton = true;
}

std::string Button::getLabelFont() const{
    ButtonComponent& btcomp = getComponent<ButtonComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(btcomp.label);

    return textcomp.font;
}

void Button::setLabelFontSize(unsigned int fontSize){
    ButtonComponent& btcomp = getComponent<ButtonComponent>();

    getLabelObject().setFontSize(fontSize);

    btcomp.needUpdateButton = true;
}

unsigned int Button::getLabelFontSize() const{
    ButtonComponent& btcomp = getComponent<ButtonComponent>();

    return getLabelObject().getFontSize();
}

void Button::setTextureNormal(std::string path){
    ButtonComponent& btcomp = getComponent<ButtonComponent>();

    btcomp.textureNormal.setPath(path);

    btcomp.needUpdateButton = true;

    setTexture(path);
}

void Button::setTexturePressed(std::string path){
    ButtonComponent& btcomp = getComponent<ButtonComponent>();

    btcomp.texturePressed.setPath(path);

    btcomp.needUpdateButton = true;
}

void Button::setTextureDisabled(std::string path){
    ButtonComponent& btcomp = getComponent<ButtonComponent>();

    btcomp.textureDisabled.setPath(path);

    btcomp.needUpdateButton = true;
}

void Button::setDisabled(bool disabled){
    ButtonComponent& btcomp = getComponent<ButtonComponent>();

    btcomp.disabled = disabled;

    btcomp.needUpdateButton = true;
}

bool Button::getDisabled() const{
    ButtonComponent& btcomp = getComponent<ButtonComponent>();

    return btcomp.disabled;
}