#include "Button.h"

#include "component/TextComponent.h"
#include "subsystem/UISystem.h"

using namespace Supernova;

Button::Button(Scene* scene): Image(scene){
    addComponent<ButtonComponent>({});

    ButtonComponent& btcomp = getComponent<ButtonComponent>();
    scene->getSystem<UISystem>()->createButtonLabel(entity, btcomp);
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