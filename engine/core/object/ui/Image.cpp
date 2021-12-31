#include "Image.h"


using namespace Supernova;

Image::Image(Scene* scene): Object(scene){
    addComponent<UIRenderComponent>({});
    addComponent<ImageComponent>({});

    UIRenderComponent& uicomp = getComponent<UIRenderComponent>();
    uicomp.buffer = &buffer;
    uicomp.indices = &indices;
    uicomp.primitiveType = PrimitiveType::TRIANGLES;

	buffer.clearAll();
	buffer.addAttribute(AttributeType::POSITION, 3);
	buffer.addAttribute(AttributeType::TEXCOORD1, 2);
    buffer.addAttribute(AttributeType::COLOR, 4);
}

void Image::setSize(int width, int height){
    UIRenderComponent& uicomp = getComponent<UIRenderComponent>();

    uicomp.width = width;
    uicomp.height = height;
}

void Image::setMargin(int margin){
    ImageComponent& img = getComponent<ImageComponent>();
    
    img.patchMarginBottom = margin;
    img.patchMarginLeft = margin;
    img.patchMarginRight = margin;
    img.patchMarginTop = margin;
}

void Image::setTexture(std::string path){
    UIRenderComponent& uicomp = getComponent<UIRenderComponent>();

    uicomp.texture.setPath(path);

    uicomp.needUpdateTexture = true;
}