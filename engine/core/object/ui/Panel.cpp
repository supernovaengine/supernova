#include "Panel.h"

#include "component/TextComponent.h"
#include "subsystem/UISystem.h"

using namespace Supernova;

Panel::Panel(Scene* scene): Image(scene){
    addComponent<PanelComponent>({});

    PanelComponent& panelcomp = getComponent<PanelComponent>();
    scene->getSystem<UISystem>()->createPanelObjects(entity, panelcomp);
}

Container Panel::getTitleContainerObject() const{
    PanelComponent& panelcomp = getComponent<PanelComponent>();

    return Container(scene, panelcomp.titlecontainer);
}

Text Panel::getTitleTextObject() const{
    PanelComponent& panelcomp = getComponent<PanelComponent>();

    return Text(scene, panelcomp.titletext);
}

void Panel::setTitle(std::string text){
    PanelComponent& panelcomp = getComponent<PanelComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(panelcomp.titletext);

    if (textcomp.text != text){
        textcomp.text = text;
        textcomp.needUpdateText = true;
    }
}

std::string Panel::getTitle() const{
    PanelComponent& panelcomp = getComponent<PanelComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(panelcomp.titletext);

    return textcomp.text;
}

void Panel::setTitleColor(Vector4 color){
    PanelComponent& panelcomp = getComponent<PanelComponent>();
    UIComponent& uititle = scene->getComponent<UIComponent>(panelcomp.titletext);

    uititle.color = color;
}

void Panel::setTitleColor(const float red, const float green, const float blue, const float alpha){
    setTitleColor(Vector4(red, green, blue, alpha));
}

Vector4 Panel::getTitleColor() const{
    PanelComponent& panelcomp = getComponent<PanelComponent>();
    UIComponent& uititle = scene->getComponent<UIComponent>(panelcomp.titletext);

    return uititle.color;
}

void Panel::setTitleFont(std::string font){
    PanelComponent& panelcomp = getComponent<PanelComponent>();

    getTitleTextObject().setFont(font);

    panelcomp.needUpdatePanel = true;
}

std::string Panel::getTitleFont() const{
    PanelComponent& panelcomp = getComponent<PanelComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(panelcomp.titletext);

    return textcomp.font;
}

void Panel::setTitleFontSize(unsigned int fontSize){
    PanelComponent& panelcomp = getComponent<PanelComponent>();

    getTitleTextObject().setFontSize(fontSize);

    panelcomp.needUpdatePanel = true;
}

unsigned int Panel::getTitleFontSize() const{
    PanelComponent& panelcomp = getComponent<PanelComponent>();

    return getTitleTextObject().getFontSize();
}