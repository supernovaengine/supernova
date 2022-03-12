//
// (c) 2022 Eduardo Doria.
//

#include "UISystem.h"

#include <locale>
#include <codecvt>
#include "Scene.h"
#include "Input.h"
#include "Engine.h"
#include "System.h"
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
    Engine::onMouseMove.remove(eventId);
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

    ui.minBufferCount = text.maxTextSize * 4;
    ui.minIndicesCount = text.maxTextSize * 6;

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

    if (!ui.loaded){
        button.textureNormal.load();
        button.texturePressed.load();
        button.textureDisabled.load();
    }

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

    if (button.disabled){
        if (ui.texture != button.textureDisabled){
            ui.texture = button.textureDisabled;
            ui.needUpdateTexture = true;
        }
    }else{
        if (!button.pressed){
            if (ui.texture != button.textureNormal){
                ui.texture = button.textureNormal;
                ui.needUpdateTexture = true;
            }
        }else{
            if (ui.texture != button.texturePressed){
                ui.texture = button.texturePressed;
                ui.needUpdateTexture = true;
            }
        }
    }
}

void UISystem::createTextEditObjects(Entity entity, TextEditComponent& textedit){
    if (textedit.text == NULL_ENTITY){
        textedit.text = scene->createEntity();

        scene->addComponent<Transform>(textedit.text, {});
        scene->addComponent<UIComponent>(textedit.text, {});
        scene->addComponent<TextComponent>(textedit.text, {});

        scene->addEntityChild(entity, textedit.text);
    }

    if (textedit.cursor == NULL_ENTITY){
        textedit.cursor = scene->createEntity();

        scene->addComponent<Transform>(textedit.cursor, {});
        scene->addComponent<UIComponent>(textedit.cursor, {});
        scene->addComponent<PolygonComponent>(textedit.cursor, {});

        scene->addEntityChild(entity, textedit.cursor);
    }

}

void UISystem::updateTextEdit(Entity entity, TextEditComponent& textedit, ImageComponent& img, UIComponent& ui){
    createTextEditObjects(entity, textedit);

    // Text
    Transform& texttransform = scene->getComponent<Transform>(textedit.text);
    UIComponent& textui = scene->getComponent<UIComponent>(textedit.text);
    TextComponent& text = scene->getComponent<TextComponent>(textedit.text);

    if (ui.height == 0){
        ui.height = textui.height + img.patchMarginTop + img.patchMarginBottom;
        img.needUpdate = true;
    }

    int heightArea = ui.height - img.patchMarginTop - img.patchMarginBottom;
    int widthArea = ui.width - img.patchMarginRight - img.patchMarginLeft - textedit.cursorWidth;

    text.multiline = false;

    int textXOffset = 0;
    if (textui.width > widthArea){
        textXOffset = textui.width - widthArea;
    }

    float textX = img.patchMarginLeft - textXOffset;
    float textY = (heightArea / 2) + (textui.height / 2);

    Vector3 textPosition = Vector3(textX, textY, 0);

    if (texttransform.position != textPosition){
        texttransform.position = textPosition;
        texttransform.needUpdate = true;
    }

    // Cursor
    Transform& cursortransform = scene->getComponent<Transform>(textedit.cursor);
    UIComponent& cursorui = scene->getComponent<UIComponent>(textedit.cursor);
    PolygonComponent& cursor = scene->getComponent<PolygonComponent>(textedit.cursor);

    float cursorHeight = textui.height;

    cursor.points.clear();
    cursor.points.push_back({Vector3(0,  0, 0),                                 Vector4(1.0, 1.0, 1.0, 1.0)});
    cursor.points.push_back({Vector3(textedit.cursorWidth, 0, 0),               Vector4(1.0, 1.0, 1.0, 1.0)});
    cursor.points.push_back({Vector3(0,  cursorHeight, 0),                      Vector4(1.0, 1.0, 1.0, 1.0)});
    cursor.points.push_back({Vector3(textedit.cursorWidth, cursorHeight, 0),    Vector4(1.0, 1.0, 1.0, 1.0)});

    float cursorX = textX + textui.width;
    float cursorY = img.patchMarginTop + ((float)heightArea / 2) - ((float)cursorHeight / 2);

    cursorui.color = textedit.cursorColor;
    cursortransform.position = Vector3(cursorX, cursorY, 0.0);
    cursortransform.needUpdate = true;

    cursor.needUpdatePolygon = true;
}

void UISystem::blinkCursorTextEdit(double dt, TextEditComponent& textedit, UIComponent& ui){
    textedit.cursorBlinkTimer += dt;

    Transform& cursortransform = scene->getComponent<Transform>(textedit.cursor);

    if (ui.focused){
        if (textedit.cursorBlinkTimer > 0.6) {
            cursortransform.visible = !cursortransform.visible;
            textedit.cursorBlinkTimer = 0;
        }
    }else{
        cursortransform.visible = false;
    }
}

void UISystem::createUIPolygon(PolygonComponent& polygon, UIComponent& ui){

    ui.primitiveType = PrimitiveType::TRIANGLE_STRIP;

    ui.buffer.clearAll();
	ui.buffer.addAttribute(AttributeType::POSITION, 3);
	ui.buffer.addAttribute(AttributeType::TEXCOORD1, 2);
    ui.buffer.addAttribute(AttributeType::COLOR, 4);
    ui.buffer.setUsage(BufferUsage::DYNAMIC);

    ui.indices.clear();

    for (int i = 0; i < polygon.points.size(); i++){
        ui.buffer.addVector3(AttributeType::POSITION, polygon.points[i].position);
        ui.buffer.addVector3(AttributeType::NORMAL, Vector3(0.0f, 0.0f, 1.0f));
        ui.buffer.addVector4(AttributeType::COLOR, polygon.points[i].color);
    }

    // Generation texcoords
    float min_X = std::numeric_limits<float>::max();
    float max_X = std::numeric_limits<float>::min();
    float min_Y = std::numeric_limits<float>::max();
    float max_Y = std::numeric_limits<float>::min();

    Attribute* attVertex = ui.buffer.getAttribute(AttributeType::POSITION);

    for (unsigned int i = 0; i < ui.buffer.getCount(); i++){
        min_X = fmin(min_X, ui.buffer.getFloat(attVertex, i, 0));
        min_Y = fmin(min_Y, ui.buffer.getFloat(attVertex, i, 1));
        max_X = fmax(max_X, ui.buffer.getFloat(attVertex, i, 0));
        max_Y = fmax(max_Y, ui.buffer.getFloat(attVertex, i, 1));
    }

    double k_X = 1/(max_X - min_X);
    double k_Y = 1/(max_Y - min_Y);

    float u = 0;
    float v = 0;

    for ( unsigned int i = 0; i < ui.buffer.getCount(); i++){
        u = (ui.buffer.getFloat(attVertex, i, 0) - min_X) * k_X;
        v = (ui.buffer.getFloat(attVertex, i, 1) - min_Y) * k_Y;

        ui.buffer.addVector2(AttributeType::TEXCOORD1, Vector2(u, v));
    }

    ui.width = (int)(max_X - min_X);
    ui.height = (int)(max_Y - min_Y);

    ui.needUpdateBuffer = true;
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
    Engine::onMouseMove.add<UISystem, &UISystem::eventOnMouseMove>(eventId, this);
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

            img.needUpdate = false;
        }
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

    //UI Polygons
    auto polygons = scene->getComponentArray<PolygonComponent>();
    for (int i = 0; i < polygons->size(); i++){
        PolygonComponent& polygon = polygons->getComponentFromIndex(i);

        if (polygon.needUpdatePolygon){
            Entity entity = polygons->getEntity(i);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<UIComponent>())){
                UIComponent& ui = scene->getComponent<UIComponent>(entity);

                createUIPolygon(polygon, ui);
            }

            polygon.needUpdatePolygon = false;
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

            button.needUpdateButton = false;
        }
    }

    // Textedits
    auto textedits = scene->getComponentArray<TextEditComponent>();
    for (int i = 0; i < textedits->size(); i++){
        TextEditComponent& textedit = textedits->getComponentFromIndex(i);

        Entity entity = textedits->getEntity(i);
        Signature signature = scene->getSignature(entity);

        if (signature.test(scene->getComponentType<UIComponent>()) && signature.test(scene->getComponentType<ImageComponent>())){
            UIComponent& ui = scene->getComponent<UIComponent>(entity);
            ImageComponent& image = scene->getComponent<ImageComponent>(entity);

            if (textedit.needUpdateTextEdit){
                updateTextEdit(entity, textedit, image, ui);

                textedit.needUpdateTextEdit = false;
            }

            blinkCursorTextEdit(dt, textedit, ui);
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

    if (signature.test(scene->getComponentType<ButtonComponent>())){
        ButtonComponent& button = scene->getComponent<ButtonComponent>(entity);

        if (button.label != NULL_ENTITY){
            scene->destroyEntity(button.label);
        }
    }
}

void UISystem::eventOnCharInput(wchar_t codepoint){
    auto uis = scene->getComponentArray<UIComponent>();
    for (int i = 0; i < uis->size(); i++){
        UIComponent& ui = uis->getComponentFromIndex(i);

        Entity entity = uis->getEntity(i);
        Signature signature = scene->getSignature(entity);
        if (signature.test(scene->getComponentType<TextEditComponent>())){
            TextEditComponent& textedit = scene->getComponent<TextEditComponent>(entity);

            if (ui.focused){
                TextComponent& text = scene->getComponent<TextComponent>(textedit.text);
                if (codepoint == '\b'){
                    if (text.text.length() > 0){
                        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > convert;
                        std::wstring utf16OldText = convert.from_bytes(text.text);

                        text.text = convert.to_bytes(utf16OldText.substr(0, utf16OldText.size() - 1));
                    }
                }else{
                    text.text = text.text + StringUtils::toUTF8(codepoint);
                }
                text.needUpdateText = true;

                textedit.needUpdateTextEdit = true;
            }
        }
    }
}

void UISystem::eventOnMouseDown(int button, float x, float y, int mods){
    auto uis = scene->getComponentArray<UIComponent>();
    int lastUI = -1;

    for (int i = 0; i < uis->size(); i++){
        UIComponent& ui = uis->getComponentFromIndex(i);

        Entity entity = uis->getEntity(i);
        Signature signature = scene->getSignature(entity);
        if (signature.test(scene->getComponentType<Transform>())){
            Transform& transform = scene->getComponent<Transform>(entity);

            if (isCoordInside(x, y, transform, ui)){
                if (signature.test(scene->getComponentType<ButtonComponent>()) || signature.test(scene->getComponentType<TextEditComponent>())){
                    lastUI = i;
                }
            }
        }

        ui.focused = false;
    }

    if (lastUI != -1){
        UIComponent& ui = uis->getComponentFromIndex(lastUI);
        Entity entity = uis->getEntity(lastUI);
        Signature signature = scene->getSignature(entity);

        if (signature.test(scene->getComponentType<ButtonComponent>())){
            ButtonComponent& button = scene->getComponent<ButtonComponent>(entity);
            if (!button.disabled){
                ui.texture = button.texturePressed;
                ui.needUpdateTexture = true;
                button.onPress.call();
                button.pressed = true;
            }
        }
        if (signature.test(scene->getComponentType<TextEditComponent>())){
            System::instance().showVirtualKeyboard();
        }else{
            System::instance().hideVirtualKeyboard();
        }

        ui.focused = true;
    }else{
        System::instance().hideVirtualKeyboard();
    }
}

void UISystem::eventOnMouseUp(int button, float x, float y, int mods){
    auto uis = scene->getComponentArray<UIComponent>();
    for (int i = 0; i < uis->size(); i++){
        UIComponent& ui = uis->getComponentFromIndex(i);

        Entity entity = uis->getEntity(i);
        Signature signature = scene->getSignature(entity);
        if (signature.test(scene->getComponentType<Transform>()) && signature.test(scene->getComponentType<ButtonComponent>())){
            ButtonComponent& button = scene->getComponent<ButtonComponent>(entity);
            if (!button.disabled){
                ui.texture = button.textureNormal;
                ui.needUpdateTexture = true;
                if (button.pressed){
                    button.pressed = false;
                    button.onRelease.call();
                }
            }
        }
    }
}

void UISystem::eventOnMouseMove(float x, float y, int mods){
    auto uis = scene->getComponentArray<UIComponent>();
    int lastUI = -1;

    for (int i = 0; i < uis->size(); i++){
        UIComponent& ui = uis->getComponentFromIndex(i);

        Entity entity = uis->getEntity(i);
        Signature signature = scene->getSignature(entity);
        if (signature.test(scene->getComponentType<Transform>())){
            Transform& transform = scene->getComponent<Transform>(entity);

            if ((!ui.mouseMoved) && isCoordInside(x, y, transform, ui)){
                lastUI = i;
            }
        }

        ui.mouseMoved = false;
    }

    if (lastUI != -1){
        UIComponent& ui = uis->getComponentFromIndex(lastUI);

        ui.onMouseMove.call();
        ui.mouseMoved = true;
    }
}

void UISystem::eventOnTouchStart(int pointer, float x, float y){
    if (!Engine::isCallMouseInTouchEvent()){
        eventOnMouseDown(S_MOUSE_BUTTON_1, x, y, 0);
    }
}

void UISystem::eventOnTouchEnd(int pointer, float x, float y){
    if (!Engine::isCallMouseInTouchEvent()){
        eventOnMouseUp(S_MOUSE_BUTTON_1, x, y, 0);
    }
}

bool UISystem::isCoordInside(float x, float y, Transform& transform, UIComponent& ui){
    Vector3 point = transform.worldRotation.getRotationMatrix() * Vector3(x, y, 0);
    Vector2 center = Vector2(0, 0);

    if (point.x >= (transform.worldPosition.x - center.x) &&
        point.x <= (transform.worldPosition.x - center.x + abs(ui.width * transform.worldScale.x)) &&
        point.y >= (transform.worldPosition.y - center.y) &&
        point.y <= (transform.worldPosition.y - center.y + abs(ui.height * transform.worldScale.y))) {
        return true;
    }
    return false;
}