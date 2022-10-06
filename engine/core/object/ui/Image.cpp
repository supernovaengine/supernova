#include "Image.h"

#include "util/Color.h"

using namespace Supernova;

Image::Image(Scene* scene): Object(scene){
    addComponent<UIComponent>({});
    addComponent<ImageComponent>({});
}

void Image::setSize(int width, int height){
    UIComponent& uicomp = getComponent<UIComponent>();
    ImageComponent& img = getComponent<ImageComponent>();

    uicomp.width = width;
    uicomp.height = height;

    img.needUpdate = true;
}

void Image::setWidth(int width){
    UIComponent& uicomp = getComponent<UIComponent>();
    ImageComponent& img = getComponent<ImageComponent>();

    uicomp.width = width;

    img.needUpdate = true;
}

void Image::setHeight(int height){
    UIComponent& uicomp = getComponent<UIComponent>();
    ImageComponent& img = getComponent<ImageComponent>();

    uicomp.height = height;

    img.needUpdate = true;
}

int Image::getWidth() const{
    UIComponent& uicomp = getComponent<UIComponent>();

    return uicomp.width;
}

int Image::getHeight() const{
    UIComponent& uicomp = getComponent<UIComponent>();

    return uicomp.height;
}

void Image::setMargin(int margin){
    ImageComponent& img = getComponent<ImageComponent>();
    
    img.patchMarginBottom = margin;
    img.patchMarginLeft = margin;
    img.patchMarginRight = margin;
    img.patchMarginTop = margin;

    img.needUpdate = true;
}

void Image::setMarginBottom(int marginBottom){
    ImageComponent& img = getComponent<ImageComponent>();
    
    img.patchMarginBottom = marginBottom;

    img.needUpdate = true;
}

void Image::setMarginLeft(int marginLeft){
    ImageComponent& img = getComponent<ImageComponent>();
    
    img.patchMarginLeft = marginLeft;

    img.needUpdate = true;
}

void Image::setMarginRight(int marginRight){
    ImageComponent& img = getComponent<ImageComponent>();
    
    img.patchMarginRight = marginRight;

    img.needUpdate = true;
}

void Image::setMarginTop(int marginTop){
    ImageComponent& img = getComponent<ImageComponent>();

    img.patchMarginTop = marginTop;

    img.needUpdate = true;
}

int Image::getMarginBottom() const{
    ImageComponent& img = getComponent<ImageComponent>();
    
    return img.patchMarginBottom;
}

int Image::getMarginLeft() const{
    ImageComponent& img = getComponent<ImageComponent>();
    
    return img.patchMarginLeft;
}

int Image::getMarginRight() const{
    ImageComponent& img = getComponent<ImageComponent>();
    
    return img.patchMarginRight;
}

int Image::getMarginTop() const{
    ImageComponent& img = getComponent<ImageComponent>();
    
    return img.patchMarginTop;
}

void Image::setTexture(std::string path){
    UIComponent& uicomp = getComponent<UIComponent>();

    uicomp.texture.setPath(path);

    uicomp.needUpdateTexture = true;
}

void Image::setTexture(FramebufferRender* framebuffer){
    UIComponent& uicomp = getComponent<UIComponent>();

    uicomp.texture.setFramebuffer(framebuffer);

    uicomp.needUpdateTexture = true;
}

void Image::setColor(Vector4 color){
    UIComponent& uicomp = getComponent<UIComponent>();

    uicomp.color = Color::sRGBToLinear(color);
}

void Image::setColor(const float red, const float green, const float blue, const float alpha){
    setColor(Vector4(red, green, blue, alpha));
}

Vector4 Image::getColor() const{
    UIComponent& uicomp = getComponent<UIComponent>();

    return Color::linearTosRGB(uicomp.color);
}