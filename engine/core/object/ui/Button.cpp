#include "Button.h"

#include "component/TextComponent.h"
#include "subsystem/UISystem.h"

using namespace Supernova;

Button::Button(Scene* scene): Image(scene){
    addComponent<ButtonComponent>({});

    ButtonComponent& btcomp = getComponent<ButtonComponent>();
    scene->getSystem<UISystem>()->createButtonLabel(entity, btcomp);

    UIComponent& uilabel = scene->getComponent<UIComponent>(btcomp.label);
    uilabel.color = Vector4(0.0, 0.0, 0.0, 1.0);
}

void Button::setLabel(std::string text){
    ButtonComponent& btcomp = getComponent<ButtonComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(btcomp.label);

    if (text.length() > textcomp.maxLength){
        text.resize(textcomp.maxLength);
        Log::Warn("Text is bigger than maxLength: %i", textcomp.maxLength);
    }

    textcomp.text = text;
    textcomp.needUpdateText = true;
}

std::string Button::getLabel(){
    ButtonComponent& btcomp = getComponent<ButtonComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(btcomp.label);

    return textcomp.text;
}

void Button::setLabelColor(Vector4 color){
    ButtonComponent& btcomp = getComponent<ButtonComponent>();
    UIComponent& uilabel = scene->getComponent<UIComponent>(btcomp.label);

    uilabel.color = color;
}

void Button::setLabelColor(float red, float green, float blue, float alpha){
    setLabelColor(Vector4(red, green, blue, alpha));
}

Vector4 Button::getLabelColor(){
    ButtonComponent& btcomp = getComponent<ButtonComponent>();
    UIComponent& uilabel = scene->getComponent<UIComponent>(btcomp.label);

    return uilabel.color;
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