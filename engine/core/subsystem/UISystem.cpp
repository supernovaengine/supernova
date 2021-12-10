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

void UISystem::loadFontAtlas(TextComponent& text, UIRenderComponent& ui){
    if (!text.stbtext){
        text.stbtext = new STBText();
    }

    TextureData* textureData = text.stbtext->load(text.font, text.fontSize);
    if (!textureData) {
        //TODO: Error
        //return false;
    }

    std::string fontId = text.font;
    if (text.font.empty())
        fontId = "font";

    ui.texture.setData(*textureData, fontId + std::string("|") + std::to_string(text.fontSize));
    ui.texture.setReleaseDataAfterLoad(true);
}

void UISystem::createText(TextComponent& text, UIRenderComponent& ui){

    ui.buffer->clear();

    std::vector<uint16_t> indices_array;

    text.stbtext->createText(text.text, ui.buffer, indices_array, text.charPositions, ui.width, ui.height, text.userDefinedWidth, text.userDefinedHeight, text.multiline, false);

    ui.indices->setValues(
            0, ui.indices->getAttribute(AttributeType::INDEX),
            indices_array.size(), (char*)&indices_array[0], sizeof(uint16_t));
}

void UISystem::load(){

}

void UISystem::draw(){

}

void UISystem::update(double dt){
    auto texts = scene->getComponentArray<TextComponent>();
    for (int i = 0; i < texts->size(); i++){
		TextComponent& text = texts->getComponentFromIndex(i);

        if (text.needUpdate){
            Entity entity = texts->getEntity(i);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<UIRenderComponent>())){
                UIRenderComponent& ui = scene->getComponent<UIRenderComponent>(entity);

                loadFontAtlas(text, ui);
                createText(text, ui);
            }


            text.needUpdate = false;
        }
    }
}

void UISystem::entityDestroyed(Entity entity){
    Signature signature = scene->getSignature(entity);

    if (signature.test(scene->getComponentType<TextComponent>())){
        TextComponent& text = scene->getComponent<TextComponent>(entity);

        if (!text.stbtext){
            delete text.stbtext;
        }
    }
}