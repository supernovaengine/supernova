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

void Image::setMargin(int margin){
    ImageComponent& img = getComponent<ImageComponent>();
    
    img.patchMarginBottom = margin;
    img.patchMarginLeft = margin;
    img.patchMarginRight = margin;
    img.patchMarginTop = margin;

    img.needUpdate = true;
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

void Image::setColor(float red, float green, float blue, float alpha){
    setColor(Vector4(red, green, blue, alpha));
}

int Image::getWidth(){
    UIComponent& uicomp = getComponent<UIComponent>();

    return uicomp.width;
}

int Image::getHeight(){
    UIComponent& uicomp = getComponent<UIComponent>();

    return uicomp.height;
}