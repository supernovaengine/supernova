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