#include "Panel.h"

#include "component/TextComponent.h"
#include "subsystem/UISystem.h"

using namespace Supernova;

Panel::Panel(Scene* scene): Image(scene){
    addComponent<PanelComponent>({});

    PanelComponent& panelcomp = getComponent<PanelComponent>();
    scene->getSystem<UISystem>()->createPanelTitle(entity, panelcomp);
}

Text Panel::getTitleObject() const{
    PanelComponent& panelcomp = getComponent<PanelComponent>();

    return Text(scene, panelcomp.title);
}

void Panel::setTitle(std::string text){
    PanelComponent& panelcomp = getComponent<PanelComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(panelcomp.title);

    if (textcomp.text != text){
        textcomp.text = text;
        textcomp.needUpdateText = true;
    }
}

std::string Panel::getTitle() const{
    PanelComponent& panelcomp = getComponent<PanelComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(panelcomp.title);

    return textcomp.text;
}

void Panel::setTitleColor(Vector4 color){
    PanelComponent& panelcomp = getComponent<PanelComponent>();
    UIComponent& uititle = scene->getComponent<UIComponent>(panelcomp.title);

    uititle.color = color;
}

void Panel::setTitleColor(const float red, const float green, const float blue, const float alpha){
    setTitleColor(Vector4(red, green, blue, alpha));
}

Vector4 Panel::getTitleColor() const{
    PanelComponent& panelcomp = getComponent<PanelComponent>();
    UIComponent& uititle = scene->getComponent<UIComponent>(panelcomp.title);

    return uititle.color;
}

void Panel::setTitleFont(std::string font){
    PanelComponent& panelcomp = getComponent<PanelComponent>();

    getTitleObject().setFont(font);

    panelcomp.needUpdatePanel = true;
}

std::string Panel::getTitleFont() const{
    PanelComponent& panelcomp = getComponent<PanelComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(panelcomp.title);

    return textcomp.font;
}

void Panel::setTitleFontSize(unsigned int fontSize){
    PanelComponent& panelcomp = getComponent<PanelComponent>();

    getTitleObject().setFontSize(fontSize);

    panelcomp.needUpdatePanel = true;
}

unsigned int Panel::getTitleFontSize() const{
    PanelComponent& panelcomp = getComponent<PanelComponent>();

    return getTitleObject().getFontSize();
}