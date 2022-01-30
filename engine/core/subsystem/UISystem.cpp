//
// (c) 2022 Eduardo Doria.
//

#include "UISystem.h"

#include "Scene.h"
#include "util/STBText.h"
#include "util/UniqueToken.h"
#include "util/StringUtils.h"

using namespace Supernova;


UISystem::UISystem(Scene* scene): SubSystem(scene){
    signature.set(scene->getComponentType<UIComponent>());

    eventId.clear();
}

UISystem::~UISystem(){
    Engine::onCharInput.remove(eventId);
    Engine::onMouseDown.remove(eventId);
    Engine::onMouseUp.remove(eventId);
    Engine::onTouchStart.remove(eventId);
    Engine::onTouchEnd.remove(eventId);
}

bool UISystem::createImagePatches(ImageComponent& img, UIComponent& ui){

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

    ui.primitiveType = PrimitiveType::TRIANGLES;

	ui.buffer.clearAll();
	ui.buffer.addAttribute(AttributeType::POSITION, 3);
	ui.buffer.addAttribute(AttributeType::TEXCOORD1, 2);
    ui.buffer.addAttribute(AttributeType::COLOR, 4);
    ui.buffer.setUsage(BufferUsage::DYNAMIC);

    ui.indices.clear();
    ui.indices.setUsage(BufferUsage::DYNAMIC);

    Attribute* atrVertex = ui.buffer.getAttribute(AttributeType::POSITION);

    ui.buffer.addVector3(atrVertex, Vector3(0, 0, 0)); //0
    ui.buffer.addVector3(atrVertex, Vector3(ui.width, 0, 0)); //1
    ui.buffer.addVector3(atrVertex, Vector3(ui.width,  ui.height, 0)); //2
    ui.buffer.addVector3(atrVertex, Vector3(0,  ui.height, 0)); //3

    ui.buffer.addVector3(atrVertex, Vector3(img.patchMarginLeft, img.patchMarginTop, 0)); //4
    ui.buffer.addVector3(atrVertex, Vector3(ui.width-img.patchMarginRight, img.patchMarginTop, 0)); //5
    ui.buffer.addVector3(atrVertex, Vector3(ui.width-img.patchMarginRight,  ui.height-img.patchMarginBottom, 0)); //6
    ui.buffer.addVector3(atrVertex, Vector3(img.patchMarginLeft,  ui.height-img.patchMarginBottom, 0)); //7

    ui.buffer.addVector3(atrVertex, Vector3(img.patchMarginLeft, 0, 0)); //8
    ui.buffer.addVector3(atrVertex, Vector3(0, img.patchMarginTop, 0)); //9

    ui.buffer.addVector3(atrVertex, Vector3(ui.width-img.patchMarginRight, 0, 0)); //10
    ui.buffer.addVector3(atrVertex, Vector3(ui.width, img.patchMarginTop, 0)); //11

    ui.buffer.addVector3(atrVertex, Vector3(ui.width-img.patchMarginRight, ui.height, 0)); //12
    ui.buffer.addVector3(atrVertex, Vector3(ui.width, ui.height-img.patchMarginBottom, 0)); //13

    ui.buffer.addVector3(atrVertex, Vector3(img.patchMarginLeft, ui.height, 0)); //14
    ui.buffer.addVector3(atrVertex, Vector3(0, ui.height-img.patchMarginBottom, 0)); //15

    Attribute* atrTexcoord = ui.buffer.getAttribute(AttributeType::TEXCOORD1);

    ui.buffer.addVector2(atrTexcoord, Vector2(0.0f, 0.0f));
    ui.buffer.addVector2(atrTexcoord, Vector2(1.0f, 0.0f));
    ui.buffer.addVector2(atrTexcoord, Vector2(1.0f, 1.0f));
    ui.buffer.addVector2(atrTexcoord, Vector2(0.0f, 1.0f));

    ui.buffer.addVector2(atrTexcoord, Vector2(img.patchMarginLeft/(float)texWidth, img.patchMarginTop/(float)texHeight));
    ui.buffer.addVector2(atrTexcoord, Vector2(1.0f-(img.patchMarginRight/(float)texWidth), img.patchMarginTop/(float)texHeight));
    ui.buffer.addVector2(atrTexcoord, Vector2(1.0f-(img.patchMarginRight/(float)texWidth), 1.0f-(img.patchMarginBottom/(float)texHeight)));
    ui.buffer.addVector2(atrTexcoord, Vector2(img.patchMarginLeft/(float)texWidth, 1.0f-(img.patchMarginBottom/(float)texHeight)));

    ui.buffer.addVector2(atrTexcoord, Vector2(img.patchMarginLeft/(float)texWidth, 0));
    ui.buffer.addVector2(atrTexcoord, Vector2(0, img.patchMarginTop/(float)texHeight));

    ui.buffer.addVector2(atrTexcoord, Vector2(1.0f-(img.patchMarginRight/(float)texWidth), 0));
    ui.buffer.addVector2(atrTexcoord, Vector2(1.0f, img.patchMarginTop/(float)texHeight));

    ui.buffer.addVector2(atrTexcoord, Vector2(1.0f-(img.patchMarginRight/(float)texWidth), 1.0f));
    ui.buffer.addVector2(atrTexcoord, Vector2(1.0f, 1.0f-(img.patchMarginBottom/(float)texHeight)));

    ui.buffer.addVector2(atrTexcoord, Vector2((img.patchMarginLeft/(float)texWidth), 1.0f));
    ui.buffer.addVector2(atrTexcoord, Vector2(0, 1.0f-(img.patchMarginBottom/(float)texHeight)));

    Attribute* atrColor = ui.buffer.getAttribute(AttributeType::COLOR);

    for (int i = 0; i < ui.buffer.getCount(); i++){
        ui.buffer.addVector4(atrColor, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
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

    ui.indices.setValues(
        0, ui.indices.getAttribute(AttributeType::INDEX),
        54, (char*)&indices_array[0], sizeof(uint16_t));

    ui.needUpdateBuffer = true;

    return true;
}

bool UISystem::loadFontAtlas(TextComponent& text, UIComponent& ui){
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

void UISystem::createText(TextComponent& text, UIComponent& ui){

    ui.primitiveType = PrimitiveType::TRIANGLES;

    ui.buffer.clearAll();
	ui.buffer.addAttribute(AttributeType::POSITION, 3);
	ui.buffer.addAttribute(AttributeType::TEXCOORD1, 2);
    ui.buffer.setUsage(BufferUsage::DYNAMIC);

    ui.indices.clear();
    ui.indices.setUsage(BufferUsage::DYNAMIC);

    ui.minBufferCount = text.maxLength * 4;
    ui.minIndicesCount = text.maxLength * 6;

    std::vector<uint16_t> indices_array;

    text.stbtext->createText(text.text, &ui.buffer, indices_array, text.charPositions, ui.width, ui.height, text.userDefinedWidth, text.userDefinedHeight, text.multiline, false);

    ui.indices.setValues(
            0, ui.indices.getAttribute(AttributeType::INDEX),
            indices_array.size(), (char*)&indices_array[0], sizeof(uint16_t));

    ui.needUpdateBuffer = true;
}

void UISystem::createButtonLabel(Entity entity, ButtonComponent& button){
    if (button.label == NULL_ENTITY){
        button.label = scene->createEntity();

        scene->addComponent<Transform>(button.label, {});
        scene->addComponent<UIComponent>(button.label, {});
        scene->addComponent<TextComponent>(button.label, {});

        scene->addEntityChild(entity, button.label);
    }
}

void UISystem::updateButton(Entity entity, ButtonComponent& button, ImageComponent& img, UIComponent& ui){

    createButtonLabel(entity, button);

    Transform& labeltransform = scene->getComponent<Transform>(button.label);
    TextComponent& labeltext = scene->getComponent<TextComponent>(button.label);
    UIComponent& labelui = scene->getComponent<UIComponent>(button.label);

    float labelX = 0;
    float labelY = (ui.height / 2) + (labelui.height / 2) - img.patchMarginBottom;

    if (labelui.width > (ui.width - img.patchMarginRight)) {
        labelX = img.patchMarginLeft;
        int labelWidth = ui.width - img.patchMarginRight;

        if (labelui.width != labelWidth){
            labelui.width = labelWidth;
            labeltext.userDefinedWidth = true;
            labeltext.needUpdateText = true;
        }
    } else {
        labelX = (ui.width / 2) - (labelui.width / 2);
    }
    Vector3 labelPosition = Vector3(labelX, labelY, 0);

    if (labeltransform.position != labelPosition){
        labeltransform.position = labelPosition;
        labeltransform.needUpdate = true;
    }
}

void UISystem::load(){
    if (eventId.empty()){
        eventId = "UIObject|" + UniqueToken::get();
    }
    // Cannot be on constructor to prevent static initialization order fiasco:
    // http://www.parashift.com/c++-faq-lite/static-init-order.html
    Engine::onCharInput.add<UISystem, &UISystem::eventOnCharInput>(eventId, this);
    Engine::onMouseDown.add<UISystem, &UISystem::eventOnMouseDown>(eventId, this);
    Engine::onMouseUp.add<UISystem, &UISystem::eventOnMouseUp>(eventId, this);
    Engine::onTouchStart.add<UISystem, &UISystem::eventOnTouchStart>(eventId, this);
    Engine::onTouchEnd.add<UISystem, &UISystem::eventOnTouchEnd>(eventId, this);
}

void UISystem::draw(){

}

void UISystem::update(double dt){

    // Images
    auto images = scene->getComponentArray<ImageComponent>();
    for (int i = 0; i < images->size(); i++){
        ImageComponent& img = images->getComponentFromIndex(i);

        if (img.needUpdate){
            Entity entity = images->getEntity(i);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<UIComponent>())){
                UIComponent& ui = scene->getComponent<UIComponent>(entity);

                createImagePatches(img, ui);
            }

            // Need to centralize button label
            if (signature.test(scene->getComponentType<ButtonComponent>())){
                ButtonComponent& button = scene->getComponent<ButtonComponent>(entity);

                button.needUpdateButton = true;
            }
        }

        img.needUpdate = false;
    }

    // Texts
    auto texts = scene->getComponentArray<TextComponent>();
    for (int i = 0; i < texts->size(); i++){
		TextComponent& text = texts->getComponentFromIndex(i);

        if (text.needUpdateText){
            Entity entity = texts->getEntity(i);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<UIComponent>())){
                UIComponent& ui = scene->getComponent<UIComponent>(entity);
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

    // Buttons
    auto buttons = scene->getComponentArray<ButtonComponent>();
    for (int i = 0; i < buttons->size(); i++){
        ButtonComponent& button = buttons->getComponentFromIndex(i);

        if (button.needUpdateButton){
            Entity entity = buttons->getEntity(i);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<UIComponent>()) && signature.test(scene->getComponentType<ImageComponent>())){
                UIComponent& ui = scene->getComponent<UIComponent>(entity);
                ImageComponent& image = scene->getComponent<ImageComponent>(entity);

                updateButton(entity, button, image, ui);
            }
        }

        button.needUpdateButton = false;
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

    if (signature.test(scene->getComponentType<ButtonComponent>())){
        ButtonComponent& button = scene->getComponent<ButtonComponent>(entity);

        if (button.label != NULL_ENTITY){
            scene->destroyEntity(button.label);
        }
    }
}

void UISystem::eventOnCharInput(wchar_t codepoint){
    //Log::Verbose("%s",StringUtils::toUTF8(codepoint).c_str());
}

void UISystem::eventOnMouseDown(int button, float x, float y, int mods){

}

void UISystem::eventOnMouseUp(int button, float x, float y, int mods){

}

void UISystem::eventOnTouchStart(int pointer, float x, float y){

}

void UISystem::eventOnTouchEnd(int pointer, float x, float y){

}