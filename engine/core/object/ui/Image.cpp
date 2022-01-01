#include "Image.h"


using namespace Supernova;

Image::Image(Scene* scene): Object(scene){
    addComponent<UIComponent>({});
    addComponent<ImageComponent>({});

    UIComponent& uicomp = getComponent<UIComponent>();
    uicomp.buffer = &buffer;
    uicomp.indices = &indices;
    uicomp.primitiveType = PrimitiveType::TRIANGLES;

	buffer.clearAll();
	buffer.addAttribute(AttributeType::POSITION, 3);
	buffer.addAttribute(AttributeType::TEXCOORD1, 2);
    buffer.addAttribute(AttributeType::COLOR, 4);
    buffer.setUsage(BufferUsage::DYNAMIC);

    indices.setUsage(BufferUsage::DYNAMIC);
}

void Image::setSize(int width, int height){
    UIComponent& uicomp = getComponent<UIComponent>();
    ImageComponent& img = getComponent<ImageComponent>();

    uicomp.width = width;
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

int Image::getWidth(){
    UIComponent& uicomp = getComponent<UIComponent>();

    return uicomp.width;
}

int Image::getHeight(){
    UIComponent& uicomp = getComponent<UIComponent>();

    return uicomp.height;
}