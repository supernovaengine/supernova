//
// (c) 2021 Eduardo Doria.
//

#include "UISystem.h"

#include "Scene.h"
#include "util/STBText.h"

using namespace Supernova;


UISystem::UISystem(Scene* scene): SubSystem(scene){
    signature.set(scene->getComponentType<UIRenderComponent>());
}

bool UISystem::createImagePatches(ImageComponent& img, UIRenderComponent& ui){

    ui.texture.load();
    unsigned int texWidth = ui.texture.getData().getWidth();
    unsigned int texHeight = ui.texture.getData().getHeight();

    if (texWidth == 0 || texHeight == 0){
        Log::Error("Cannot create UI image without texture");
        return false;
    }

    if (ui.width == 0 && ui.height == 0){
        ui.width = texWidth;
        ui.height = texHeight;
    }

    ui.buffer->clear();
    ui.indices->clear();

    Attribute* atrVertex = ui.buffer->getAttribute(AttributeType::POSITION);

    ui.buffer->addVector3(atrVertex, Vector3(0, 0, 0)); //0
    ui.buffer->addVector3(atrVertex, Vector3(ui.width, 0, 0)); //1
    ui.buffer->addVector3(atrVertex, Vector3(ui.width,  ui.height, 0)); //2
    ui.buffer->addVector3(atrVertex, Vector3(0,  ui.height, 0)); //3

    ui.buffer->addVector3(atrVertex, Vector3(img.patchMarginLeft, img.patchMarginTop, 0)); //4
    ui.buffer->addVector3(atrVertex, Vector3(ui.width-img.patchMarginRight, img.patchMarginTop, 0)); //5
    ui.buffer->addVector3(atrVertex, Vector3(ui.width-img.patchMarginRight,  ui.height-img.patchMarginBottom, 0)); //6
    ui.buffer->addVector3(atrVertex, Vector3(img.patchMarginLeft,  ui.height-img.patchMarginBottom, 0)); //7

    ui.buffer->addVector3(atrVertex, Vector3(img.patchMarginLeft, 0, 0)); //8
    ui.buffer->addVector3(atrVertex, Vector3(0, img.patchMarginTop, 0)); //9

    ui.buffer->addVector3(atrVertex, Vector3(ui.width-img.patchMarginRight, 0, 0)); //10
    ui.buffer->addVector3(atrVertex, Vector3(ui.width, img.patchMarginTop, 0)); //11

    ui.buffer->addVector3(atrVertex, Vector3(ui.width-img.patchMarginRight, ui.height, 0)); //12
    ui.buffer->addVector3(atrVertex, Vector3(ui.width, ui.height-img.patchMarginBottom, 0)); //13

    ui.buffer->addVector3(atrVertex, Vector3(img.patchMarginLeft, ui.height, 0)); //14
    ui.buffer->addVector3(atrVertex, Vector3(0, ui.height-img.patchMarginBottom, 0)); //15

    Attribute* atrTexcoord = ui.buffer->getAttribute(AttributeType::TEXCOORD1);

    ui.buffer->addVector2(atrTexcoord, Vector2(0.0f, 0.0f));
    ui.buffer->addVector2(atrTexcoord, Vector2(1.0f, 0.0f));
    ui.buffer->addVector2(atrTexcoord, Vector2(1.0f, 1.0f));
    ui.buffer->addVector2(atrTexcoord, Vector2(0.0f, 1.0f));

    ui.buffer->addVector2(atrTexcoord, Vector2(img.patchMarginLeft/(float)ui.width, img.patchMarginTop/(float)ui.height));
    ui.buffer->addVector2(atrTexcoord, Vector2(1.0f-(img.patchMarginRight/(float)ui.width), img.patchMarginTop/(float)ui.height));
    ui.buffer->addVector2(atrTexcoord, Vector2(1.0f-(img.patchMarginRight/(float)ui.width), 1.0f-(img.patchMarginBottom/(float)ui.height)));
    ui.buffer->addVector2(atrTexcoord, Vector2(img.patchMarginLeft/(float)ui.width, 1.0f-(img.patchMarginBottom/(float)ui.height)));

    ui.buffer->addVector2(atrTexcoord, Vector2(img.patchMarginLeft/(float)ui.width, 0));
    ui.buffer->addVector2(atrTexcoord, Vector2(0, img.patchMarginTop/(float)ui.height));

    ui.buffer->addVector2(atrTexcoord, Vector2(1.0f-(img.patchMarginRight/(float)ui.width), 0));
    ui.buffer->addVector2(atrTexcoord, Vector2(1.0f, img.patchMarginTop/(float)ui.height));

    ui.buffer->addVector2(atrTexcoord, Vector2(1.0f-(img.patchMarginRight/(float)ui.width), 1.0f));
    ui.buffer->addVector2(atrTexcoord, Vector2(1.0f, 1.0f-(img.patchMarginBottom/(float)ui.height)));

    ui.buffer->addVector2(atrTexcoord, Vector2((img.patchMarginLeft/(float)ui.width), 1.0f));
    ui.buffer->addVector2(atrTexcoord, Vector2(0, 1.0f-(img.patchMarginBottom/(float)ui.height)));

    Attribute* atrColor = ui.buffer->getAttribute(AttributeType::COLOR);

    for (int i = 0; i < ui.buffer->getCount(); i++){
        ui.buffer->addVector4(atrColor, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    static const uint16_t indices_array[] = {
        4,  5,  6, // Center
        4,  6,  7,

        0,  8,  4, // Top-left
        0,  4,  9,

        8, 10,  5, // Top
        8,  5,  4,

        10, 1, 11, // Top-right
        10, 11, 5,

        5, 11, 13, // Right
        5, 13,  6,

        6, 13,  2, // Bottom-right
        6,  2, 12,

        7,  6, 12, // Bottom
        7, 12, 14,

        15, 7, 14, // Bottom-left
        15, 14, 3,

        9,  4,  7, // Left
        9,  7, 15
    };

    ui.indices->setValues(
        0, ui.indices->getAttribute(AttributeType::INDEX),
        54, (char*)&indices_array[0], sizeof(uint16_t));

    return true;
}

bool UISystem::loadFontAtlas(TextComponent& text, UIRenderComponent& ui){
    if (!text.stbtext){
        text.stbtext = new STBText();
    }

    TextureData* textureData = text.stbtext->load(text.font, text.fontSize);
    if (!textureData) {
        Log::Error("Cannot load font atlas from: %s", text.font.c_str());
        return false;
    }

    std::string fontId = text.font;
    if (text.font.empty())
        fontId = "font";

    ui.texture.setData(*textureData, fontId + std::string("|") + std::to_string(text.fontSize));
    ui.texture.setReleaseDataAfterLoad(true);

    ui.needUpdateTexture = true;

    text.needReload = false;
    text.loaded = true;

    return true;
}

void UISystem::createText(TextComponent& text, UIRenderComponent& ui){

    ui.buffer->clear();
    ui.indices->clear();

    std::vector<uint16_t> indices_array;

    text.stbtext->createText(text.text, ui.buffer, indices_array, text.charPositions, ui.width, ui.height, text.userDefinedWidth, text.userDefinedHeight, text.multiline, false);

    ui.indices->setValues(
            0, ui.indices->getAttribute(AttributeType::INDEX),
            indices_array.size(), (char*)&indices_array[0], sizeof(uint16_t));

    ui.needUpdateBuffer = true;
}

void UISystem::load(){

}

void UISystem::draw(){

}

void UISystem::update(double dt){
    auto images = scene->getComponentArray<ImageComponent>();
    for (int i = 0; i < images->size(); i++){
        ImageComponent& img = images->getComponentFromIndex(i);

        if (img.needUpdate){
            Entity entity = images->getEntity(i);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<UIRenderComponent>())){
                UIRenderComponent& ui = scene->getComponent<UIRenderComponent>(entity);

                createImagePatches(img, ui);
            }
        }

        img.needUpdate = false;
    }

    auto texts = scene->getComponentArray<TextComponent>();
    for (int i = 0; i < texts->size(); i++){
		TextComponent& text = texts->getComponentFromIndex(i);

        if (text.needUpdateText){
            Entity entity = texts->getEntity(i);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<UIRenderComponent>())){
                UIRenderComponent& ui = scene->getComponent<UIRenderComponent>(entity);
                if (text.loaded && text.needReload){
                    ui.texture.destroy(); //texture.setData also destroy it
                    text.loaded = false;
                }
                if (!text.loaded){
                    loadFontAtlas(text, ui);
                }
                createText(text, ui);
            }


            text.needUpdateText = false;
        }
    }
}

void UISystem::entityDestroyed(Entity entity){
    Signature signature = scene->getSignature(entity);

    if (signature.test(scene->getComponentType<TextComponent>())){
        TextComponent& text = scene->getComponent<TextComponent>(entity);

        if (text.stbtext){
            delete text.stbtext;
        }
    }
}