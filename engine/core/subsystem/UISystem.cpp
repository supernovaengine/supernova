//
// (c) 2022 Eduardo Doria.
//

#include "UISystem.h"

#include <locale>
#include <codecvt>
#include <algorithm>
#include "Scene.h"
#include "Input.h"
#include "Engine.h"
#include "System.h"
#include "util/STBText.h"
#include "util/StringUtils.h"

using namespace Supernova;


UISystem::UISystem(Scene* scene): SubSystem(scene){
    signature.set(scene->getComponentType<UILayoutComponent>());

    eventId.clear();
}

UISystem::~UISystem(){
}

bool UISystem::createImagePatches(ImageComponent& img, UIComponent& ui, UILayoutComponent& layout){

    ui.texture.load();
    unsigned int texWidth = ui.texture.getWidth();
    unsigned int texHeight = ui.texture.getHeight();

    if (texWidth == 0 || texHeight == 0){
        Log::warn("Cannot create UI image without texture");
        return false;
    }

    if (layout.width == 0 && layout.height == 0){
        layout.width = texWidth;
        layout.height = texHeight;
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
    ui.buffer.addVector3(atrVertex, Vector3(layout.width, 0, 0)); //1
    ui.buffer.addVector3(atrVertex, Vector3(layout.width,  layout.height, 0)); //2
    ui.buffer.addVector3(atrVertex, Vector3(0,  layout.height, 0)); //3

    ui.buffer.addVector3(atrVertex, Vector3(img.patchMarginLeft, img.patchMarginTop, 0)); //4
    ui.buffer.addVector3(atrVertex, Vector3(layout.width-img.patchMarginRight, img.patchMarginTop, 0)); //5
    ui.buffer.addVector3(atrVertex, Vector3(layout.width-img.patchMarginRight,  layout.height-img.patchMarginBottom, 0)); //6
    ui.buffer.addVector3(atrVertex, Vector3(img.patchMarginLeft,  layout.height-img.patchMarginBottom, 0)); //7

    ui.buffer.addVector3(atrVertex, Vector3(img.patchMarginLeft, 0, 0)); //8
    ui.buffer.addVector3(atrVertex, Vector3(0, img.patchMarginTop, 0)); //9

    ui.buffer.addVector3(atrVertex, Vector3(layout.width-img.patchMarginRight, 0, 0)); //10
    ui.buffer.addVector3(atrVertex, Vector3(layout.width, img.patchMarginTop, 0)); //11

    ui.buffer.addVector3(atrVertex, Vector3(layout.width-img.patchMarginRight, layout.height, 0)); //12
    ui.buffer.addVector3(atrVertex, Vector3(layout.width, layout.height-img.patchMarginBottom, 0)); //13

    ui.buffer.addVector3(atrVertex, Vector3(img.patchMarginLeft, layout.height, 0)); //14
    ui.buffer.addVector3(atrVertex, Vector3(0, layout.height-img.patchMarginBottom, 0)); //15

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

    if (ui.flipY){
        for (int i = 0; i < ui.buffer.getCount(); i++){
            Vector2 uv = ui.buffer.getVector2(atrTexcoord, i);
            uv.y = 1.0 - uv.y;
            ui.buffer.setVector2(i, atrTexcoord, uv);
        }
    }

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

bool UISystem::loadFontAtlas(TextComponent& text, UIComponent& ui, UILayoutComponent& layout){
    if (!text.stbtext){
        text.stbtext = new STBText();
    }

    TextureData* textureData = text.stbtext->load(text.font, text.fontSize);
    if (!textureData) {
        Log::error("Cannot load font atlas from: %s", text.font.c_str());
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

void UISystem::createText(TextComponent& text, UIComponent& ui, UILayoutComponent& layout){

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

    text.stbtext->createText(text.text, &ui.buffer, indices_array, text.charPositions, layout.width, layout.height, text.fixedWidth, text.fixedHeight, text.multiline, ui.flipY);

    ui.indices.setValues(
            0, ui.indices.getAttribute(AttributeType::INDEX),
            indices_array.size(), (char*)&indices_array[0], sizeof(uint16_t));

    ui.needUpdateBuffer = true;
}

void UISystem::createButtonLabel(Entity entity, ButtonComponent& button){
    if (button.label == NULL_ENTITY){
        button.label = scene->createEntity();

        scene->addComponent<Transform>(button.label, {});
        scene->addComponent<UILayoutComponent>(button.label, {});
        scene->addComponent<UIComponent>(button.label, {});
        scene->addComponent<TextComponent>(button.label, {});

        scene->addEntityChild(entity, button.label);
    }
}

void UISystem::updateButton(Entity entity, ButtonComponent& button, ImageComponent& img, UIComponent& ui, UILayoutComponent& layout){
    createButtonLabel(entity, button);

    if (!ui.loaded){
        if (!button.textureNormal.load()){
            button.textureNormal = ui.texture;
            button.textureNormal.load();
        }
        button.texturePressed.load();
        button.textureDisabled.load();
    }

    Transform& labeltransform = scene->getComponent<Transform>(button.label);
    TextComponent& labeltext = scene->getComponent<TextComponent>(button.label);
    UIComponent& labelui = scene->getComponent<UIComponent>(button.label);
    UILayoutComponent& labellayout = scene->getComponent<UILayoutComponent>(button.label);

    loadOrUpdateText(labeltext, labelui, labellayout);
    
    labellayout.anchorPreset = AnchorPreset::CENTER;
    labellayout.needUpdateAnchors = true;

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
        scene->addComponent<UILayoutComponent>(textedit.text, {});
        scene->addComponent<UIComponent>(textedit.text, {});
        scene->addComponent<TextComponent>(textedit.text, {});

        scene->addEntityChild(entity, textedit.text);
    }

    if (textedit.cursor == NULL_ENTITY){
        textedit.cursor = scene->createEntity();

        scene->addComponent<Transform>(textedit.cursor, {});
        scene->addComponent<UILayoutComponent>(textedit.cursor, {});
        scene->addComponent<UIComponent>(textedit.cursor, {});
        scene->addComponent<PolygonComponent>(textedit.cursor, {});

        scene->addEntityChild(entity, textedit.cursor);
    }

}

void UISystem::updateTextEdit(Entity entity, TextEditComponent& textedit, ImageComponent& img, UIComponent& ui, UILayoutComponent& layout){
    createTextEditObjects(entity, textedit);

    // Text
    Transform& texttransform = scene->getComponent<Transform>(textedit.text);
    UILayoutComponent& textlayout = scene->getComponent<UILayoutComponent>(textedit.text);
    UIComponent& textui = scene->getComponent<UIComponent>(textedit.text);
    TextComponent& text = scene->getComponent<TextComponent>(textedit.text);

    loadOrUpdateText(text, textui, textlayout);

    if (layout.height == 0){
        layout.height = textlayout.height + img.patchMarginTop + img.patchMarginBottom;
        layout.needUpdateSizes = true;
    }

    int heightArea = layout.height - img.patchMarginTop - img.patchMarginBottom;
    int widthArea = layout.width - img.patchMarginRight - img.patchMarginLeft - textedit.cursorWidth;

    text.multiline = false;

    int textXOffset = 0;
    if (textlayout.width > widthArea){
        textXOffset = textlayout.width - widthArea;
    }

    float textX = img.patchMarginLeft - textXOffset;
    // descend is negative
    float textY = img.patchMarginTop + (heightArea / 2) - (textlayout.height / 2);

    Vector3 textPosition = Vector3(textX, textY, 0);

    if (texttransform.position != textPosition){
        texttransform.position = textPosition;
        texttransform.needUpdate = true;
    }

    // Cursor
    Transform& cursortransform = scene->getComponent<Transform>(textedit.cursor);
    UILayoutComponent& cursorlayout = scene->getComponent<UILayoutComponent>(textedit.cursor);
    UIComponent& cursorui = scene->getComponent<UIComponent>(textedit.cursor);
    PolygonComponent& cursor = scene->getComponent<PolygonComponent>(textedit.cursor);

    loadOrUpdatePolygon(cursor, cursorui, cursorlayout);

    float cursorHeight = textlayout.height;

    cursor.points.clear();
    cursor.points.push_back({Vector3(0,  0, 0),                                 Vector4(1.0, 1.0, 1.0, 1.0)});
    cursor.points.push_back({Vector3(textedit.cursorWidth, 0, 0),               Vector4(1.0, 1.0, 1.0, 1.0)});
    cursor.points.push_back({Vector3(0,  cursorHeight, 0),                      Vector4(1.0, 1.0, 1.0, 1.0)});
    cursor.points.push_back({Vector3(textedit.cursorWidth, cursorHeight, 0),    Vector4(1.0, 1.0, 1.0, 1.0)});

    float cursorX = textX + textlayout.width;
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

void UISystem::createUIPolygon(PolygonComponent& polygon, UIComponent& ui, UILayoutComponent& layout){

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

        if (ui.flipY)
            v = 1.0 - v;
        ui.buffer.addVector2(AttributeType::TEXCOORD1, Vector2(u, v));
    }

    layout.width = (int)(max_X - min_X);
    layout.height = (int)(max_Y - min_Y);

    ui.needUpdateBuffer = true;
}

bool UISystem::loadOrUpdatePolygon(PolygonComponent& polygon, UIComponent& ui, UILayoutComponent& layout){
    if (polygon.needUpdatePolygon){
        createUIPolygon(polygon, ui, layout);

        polygon.needUpdatePolygon = false;
    }

    return true;
}

bool UISystem::loadOrUpdateImage(ImageComponent& img, UIComponent& ui, UILayoutComponent& layout){

    if (img.needUpdatePatches){
        createImagePatches(img, ui, layout);

        img.needUpdatePatches = false;
    }

    return true;
}

bool UISystem::loadOrUpdateText(TextComponent& text, UIComponent& ui, UILayoutComponent& layout){
    if (text.needUpdateText){
        if (text.loaded && text.needReload){
            ui.texture.destroy(); //texture.setData also destroy it
            text.loaded = false;
        }
        if (!text.loaded){
            loadFontAtlas(text, ui, layout);
        }
        createText(text, ui, layout);

        layout.needUpdateAnchors = true;

        text.needUpdateText = false;
    }

    return true;
}

void UISystem::updateAllAnchors(){
    auto layouts = scene->getComponentArray<UILayoutComponent>();
    for (int i = 0; i < layouts->size(); i++){
        UILayoutComponent& layout = layouts->getComponentFromIndex(i);
        Entity entity = layouts->getEntity(i);
        Signature signature = scene->getSignature(entity);
        if (signature.test(scene->getComponentType<UIComponent>())){
            UIComponent& ui = scene->getComponent<UIComponent>(entity);
            if (ui.loaded){
                layout.needUpdateAnchors = true;
            }
        }else{
            layout.needUpdateAnchors = true;
        }
    }
}

void UISystem::applyAnchorPreset(UILayoutComponent& layout){
    if (layout.anchorPreset == AnchorPreset::TOP_LEFT){
        layout.anchorLeft = 0;
        layout.anchorTop = 0;
        layout.anchorRight = 0;
        layout.anchorBottom = 0;
        layout.marginLeft = 0;
        layout.marginTop = 0;
        layout.marginRight = layout.width;
        layout.marginBottom = layout.height;
    }else if (layout.anchorPreset == AnchorPreset::TOP_RIGHT){
        layout.anchorLeft = 1;
        layout.anchorTop = 0;
        layout.anchorRight = 1;
        layout.anchorBottom = 0;
        layout.marginLeft = -layout.width;
        layout.marginTop = 0;
        layout.marginRight = 0;
        layout.marginBottom = layout.height;
    }else if (layout.anchorPreset == AnchorPreset::BOTTOM_RIGHT){
        layout.anchorLeft = 1;
        layout.anchorTop = 1;
        layout.anchorRight = 1;
        layout.anchorBottom = 1;
        layout.marginLeft = -layout.width;
        layout.marginTop = -layout.height;
        layout.marginRight = 0;
        layout.marginBottom = 0;
    }else if (layout.anchorPreset == AnchorPreset::BOTTOM_LEFT){
        layout.anchorLeft = 0;
        layout.anchorTop = 1;
        layout.anchorRight = 0;
        layout.anchorBottom = 1;
        layout.marginLeft = 0;
        layout.marginTop = -layout.height;
        layout.marginRight = layout.width;
        layout.marginBottom = 0;
    }else if (layout.anchorPreset == AnchorPreset::CENTER_LEFT){
        layout.anchorLeft = 0;
        layout.anchorTop = 0.5;
        layout.anchorRight = 0;
        layout.anchorBottom = 0.5;
        layout.marginLeft = 0;
        layout.marginTop = -layout.height / 2;
        layout.marginRight = layout.width;
        layout.marginBottom = layout.height / 2;
    }else if (layout.anchorPreset == AnchorPreset::CENTER_TOP){
        layout.anchorLeft = 0.5;
        layout.anchorTop = 0;
        layout.anchorRight = 0.5;
        layout.anchorBottom = 0;
        layout.marginLeft = -layout.width / 2;
        layout.marginTop = 0;
        layout.marginRight = layout.width / 2;
        layout.marginBottom = layout.height;
    }else if (layout.anchorPreset == AnchorPreset::CENTER_RIGHT){
        layout.anchorLeft = 1;
        layout.anchorTop = 0.5;
        layout.anchorRight = 1;
        layout.anchorBottom = 0.5;
        layout.marginLeft = -layout.width;
        layout.marginTop = -layout.height / 2;
        layout.marginRight = 0;
        layout.marginBottom = layout.height / 2;
    }else if (layout.anchorPreset == AnchorPreset::CENTER_BOTTOM){
        layout.anchorLeft = 0.5;
        layout.anchorTop = 1;
        layout.anchorRight = 0.5;
        layout.anchorBottom = 1;
        layout.marginLeft = -layout.width / 2;
        layout.marginTop = -layout.height;
        layout.marginRight = layout.width / 2;
        layout.marginBottom = 0;
    }else if (layout.anchorPreset == AnchorPreset::CENTER){
        layout.anchorLeft = 0.5;
        layout.anchorTop = 0.5;
        layout.anchorRight = 0.5;
        layout.anchorBottom = 0.5;
        layout.marginLeft = -layout.width / 2;
        layout.marginTop = -layout.height / 2;
        layout.marginRight = layout.width / 2;
        layout.marginBottom = layout.height / 2;
    }else if (layout.anchorPreset == AnchorPreset::LEFT_WIDE){
        layout.anchorLeft = 0;
        layout.anchorTop = 0;
        layout.anchorRight = 0;
        layout.anchorBottom = 1;
        layout.marginLeft = 0;
        layout.marginTop = 0;
        layout.marginRight = layout.width;
        layout.marginBottom = 0;
    }else if (layout.anchorPreset == AnchorPreset::TOP_WIDE){
        layout.anchorLeft = 0;
        layout.anchorTop = 0;
        layout.anchorRight = 1;
        layout.anchorBottom = 0;
        layout.marginLeft = 0;
        layout.marginTop = 0;
        layout.marginRight = 0;
        layout.marginBottom = layout.height;
    }else if (layout.anchorPreset == AnchorPreset::RIGHT_WIDE){
        layout.anchorLeft = 1;
        layout.anchorTop = 0;
        layout.anchorRight = 1;
        layout.anchorBottom = 1;
        layout.marginLeft = -layout.width;
        layout.marginTop = 0;
        layout.marginRight = 0;
        layout.marginBottom = 0;
    }else if (layout.anchorPreset == AnchorPreset::BOTTOM_WIDE){
        layout.anchorLeft = 0;
        layout.anchorTop = 1;
        layout.anchorRight = 1;
        layout.anchorBottom = 1;
        layout.marginLeft = 0;
        layout.marginTop = -layout.height;
        layout.marginRight = 0;
        layout.marginBottom = 0;
    }else if (layout.anchorPreset == AnchorPreset::VERTICAL_CENTER_WIDE){
        layout.anchorLeft = 0.5;
        layout.anchorTop = 0;
        layout.anchorRight = 0.5;
        layout.anchorBottom = 1;
        layout.marginLeft = -layout.width / 2;
        layout.marginTop = 0;
        layout.marginRight = layout.width / 2;
        layout.marginBottom = 0;
    }else if (layout.anchorPreset == AnchorPreset::HORIZONTAL_CENTER_WIDE){
        layout.anchorLeft = 0;
        layout.anchorTop = 0.5;
        layout.anchorRight = 1;
        layout.anchorBottom = 0.5;
        layout.marginLeft = 0;
        layout.marginTop = -layout.height / 2;
        layout.marginRight = 0;
        layout.marginBottom = layout.height / 2;
    }else if (layout.anchorPreset == AnchorPreset::FULL_LAYOUT){
        layout.anchorLeft = 0;
        layout.anchorTop = 0;
        layout.anchorRight = 1;
        layout.anchorBottom = 1;
        layout.marginLeft = 0;
        layout.marginTop = 0;
        layout.marginRight = 0;
        layout.marginBottom = 0;
    }
}

void UISystem::changeFlipY(UIComponent& ui, CameraComponent& camera){
    ui.flipY = false;
    if (camera.type != CameraType::CAMERA_2D){
        ui.flipY = !ui.flipY;
    }

    if (ui.texture.isFramebuffer() && Engine::isOpenGL()){
        ui.flipY = !ui.flipY;
    }
}

void UISystem::load(){
}

void UISystem::destroy(){
}

void UISystem::draw(){
}

void UISystem::update(double dt){

    // need to be ordered by Transform
    auto layouts = scene->getComponentArray<UILayoutComponent>();

    // reverse Transform order
    for (int i = layouts->size()-1; i >= 0; i--){
        UILayoutComponent& layout = layouts->getComponentFromIndex(i);
        Entity entity = layouts->getEntity(i);
        Signature signature = scene->getSignature(entity);

        if (signature.test(scene->getComponentType<UIContainerComponent>())){
            UIContainerComponent& container = scene->getComponent<UIContainerComponent>(entity);
            // configuring all container boxes
            if (container.numBoxes > 0){

                if (layout.width == 0 || layout.height == 0){
                    int genWidth = 0;
                    int genHeight = 0;
                    for (int b = 0; b < container.numBoxes; b++){
                        if (container.boxes[b].layout != NULL_ENTITY){
                            if (container.type == ContainerType::HORIZONTAL){
                                genWidth += container.boxes[b].rect.getWidth();
                                genHeight = std::max(genHeight, (int)container.boxes[b].rect.getHeight());
                            }else if (container.type == ContainerType::VERTICAL){
                                genWidth = std::max(genWidth, (int)container.boxes[b].rect.getWidth()); 
                                genHeight += container.boxes[b].rect.getHeight();
                            }
                        }
                    }
                    layout.width = (layout.width > 0)? layout.width : genWidth;
                    layout.height = (layout.height > 0)? layout.height : genHeight;
                }

                int totalWidth = layout.width;
                int totalHeight = layout.height;

                for (int b = (container.numBoxes-1); b >= 0; b--){
                    if (container.boxes[b].layout != NULL_ENTITY){
                        // container.boxes[b].rect in setted by entity size in last iteration
                        if (container.type == ContainerType::HORIZONTAL){
                            if (b < (container.numBoxes-1)){
                                container.boxes[b].rect.setX(container.boxes[b+1].rect.getX() + container.boxes[b+1].rect.getWidth());
                            }
                            if (container.boxes[b].expand && layout.anchorPreset != AnchorPreset::NONE){
                                container.boxes[b].rect.setWidth(totalWidth / (b+1));
                            }
                            container.boxes[b].rect.setHeight(layout.height);

                            totalWidth = totalWidth - container.boxes[b].rect.getWidth();
                        }else if (container.type == ContainerType::VERTICAL){
                            if (b < (container.numBoxes-1)){
                                container.boxes[b].rect.setY(container.boxes[b+1].rect.getY() + container.boxes[b+1].rect.getHeight());
                            }
                            if (container.boxes[b].expand && layout.anchorPreset != AnchorPreset::NONE){
                                container.boxes[b].rect.setHeight(totalHeight / (b+1));
                            }
                            container.boxes[b].rect.setWidth(layout.width);

                            totalHeight = totalHeight - container.boxes[b].rect.getHeight();
                        }
                    }
                }
            }
        }

        if (signature.test(scene->getComponentType<Transform>())){
            Transform& transform = scene->getComponent<Transform>(entity);

            UILayoutComponent* parentlayout = scene->findComponent<UILayoutComponent>(transform.parent);
            if (parentlayout){
                if (parentlayout->needUpdateAnchors || parentlayout->needUpdateSizes){
                    layout.needUpdateAnchors = true;
                }

                UIContainerComponent* parentcontainer = scene->findComponent<UIContainerComponent>(transform.parent);
                if (parentcontainer){
                    if (parentcontainer->numBoxes < MAX_CONTAINER_BOXES){
                        layout.containerBoxIndex = parentcontainer->numBoxes;
                        parentcontainer->boxes[layout.containerBoxIndex].layout = entity;
                        parentcontainer->boxes[layout.containerBoxIndex].rect = Rect(0, 0, layout.width, layout.height);

                        parentcontainer->numBoxes = parentcontainer->numBoxes + 1;
                    }else{
                        transform.parent = NULL_ENTITY;
                        Log::error("The UI container has exceeded the maximum allowed of %i children. Please, increase MAX_CONTAINER_BOXES value.", MAX_CONTAINER_BOXES);
                    }
                }
            }
        }


    }

    for (int i = 0; i < layouts->size(); i++){
        UILayoutComponent& layout = layouts->getComponentFromIndex(i);
        Entity entity = layouts->getEntity(i);
        Signature signature = scene->getSignature(entity);

        if (layout.needUpdateSizes){
            if (signature.test(scene->getComponentType<ImageComponent>())){
                ImageComponent& img = scene->getComponent<ImageComponent>(entity);

                img.needUpdatePatches = true;
            }

            if (signature.test(scene->getComponentType<TextComponent>())){
                TextComponent& text = scene->getComponent<TextComponent>(entity);

                text.needUpdateText = true;
            }

            layout.needUpdateSizes = false;
            layout.needUpdateAnchors = true;
        }

        if (signature.test(scene->getComponentType<UIComponent>())){
            UIComponent& ui = scene->getComponent<UIComponent>(entity);

            if (ui.automaticFlipY){
                CameraComponent& camera = scene->getComponent<CameraComponent>(scene->getCamera());
                changeFlipY(ui, camera);
            }

            // Texts
            if (signature.test(scene->getComponentType<TextComponent>())){
                TextComponent& text = scene->getComponent<TextComponent>(entity);

                loadOrUpdateText(text, ui, layout);
            }

            // UI Polygons
            if (signature.test(scene->getComponentType<PolygonComponent>())){
                PolygonComponent& polygon = scene->getComponent<PolygonComponent>(entity);

                loadOrUpdatePolygon(polygon, ui, layout);
            }

            // Images
            if (signature.test(scene->getComponentType<ImageComponent>())){
                ImageComponent& img = scene->getComponent<ImageComponent>(entity);

                loadOrUpdateImage(img, ui, layout);

                // Buttons
                if (signature.test(scene->getComponentType<ButtonComponent>())){
                    ButtonComponent& button = scene->getComponent<ButtonComponent>(entity);

                    if (button.needUpdateButton){
                        updateButton(entity, button, img, ui, layout);
                        
                        button.needUpdateButton = false;
                    }
                }

                // Textedits
                if (signature.test(scene->getComponentType<TextEditComponent>())){
                    TextEditComponent& textedit = scene->getComponent<TextEditComponent>(entity);

                    if (textedit.needUpdateTextEdit){
                        updateTextEdit(entity, textedit, img, ui, layout);

                        textedit.needUpdateTextEdit = false;
                    }

                    blinkCursorTextEdit(dt, textedit, ui);
                }
            }

        }

        if (signature.test(scene->getComponentType<Transform>())){
            Transform& transform = scene->getComponent<Transform>(entity);

            if (layout.needUpdateAnchors){
                if (layout.anchorRight < layout.anchorLeft)
                    layout.anchorRight = layout.anchorLeft;
                if (layout.anchorBottom < layout.anchorTop)
                    layout.anchorBottom = layout.anchorTop;

                applyAnchorPreset(layout);
            }

            float abAnchorLeft = 0;
            float abAnchorRight = 0;
            float abAnchorTop = 0;
            float abAnchorBottom = 0;
            if (transform.parent == NULL_ENTITY){
                abAnchorLeft = Engine::getCanvasWidth() * layout.anchorLeft;
                abAnchorRight = Engine::getCanvasWidth() * layout.anchorRight;
                abAnchorTop = Engine::getCanvasHeight() * layout.anchorTop;
                abAnchorBottom = Engine::getCanvasHeight() * layout.anchorBottom;
            }else{
                UILayoutComponent* parentlayout = scene->findComponent<UILayoutComponent>(transform.parent);
                if (parentlayout){
                    Rect boxRect = Rect(0, 0, parentlayout->width, parentlayout->height);

                    UIContainerComponent* parentcontainer = scene->findComponent<UIContainerComponent>(transform.parent);
                    if (parentcontainer){
                        boxRect = parentcontainer->boxes[layout.containerBoxIndex].rect;
                    }

                    ImageComponent* parentimage = scene->findComponent<ImageComponent>(transform.parent);
                    if (parentimage){
                        boxRect.setX(boxRect.getX() + parentimage->patchMarginLeft);
                        boxRect.setWidth(boxRect.getWidth() - parentimage->patchMarginRight - parentimage->patchMarginLeft);
                        boxRect.setY(boxRect.getY() + parentimage->patchMarginTop);
                        boxRect.setHeight(boxRect.getHeight() - parentimage->patchMarginBottom - parentimage->patchMarginTop);
                    }

                    abAnchorLeft = (boxRect.getWidth() * layout.anchorLeft) + boxRect.getX();
                    abAnchorRight = (boxRect.getWidth() * layout.anchorRight) + boxRect.getX();
                    abAnchorTop = (boxRect.getHeight() * layout.anchorTop) + boxRect.getY();
                    abAnchorBottom = (boxRect.getHeight() * layout.anchorBottom) + boxRect.getY();
                }
            }

            if (layout.needUpdateAnchors){
                transform.position.x = abAnchorLeft + layout.marginLeft;
                transform.position.y = abAnchorTop + layout.marginTop;
                transform.needUpdate = true;

                float width = abAnchorRight - transform.position.x + layout.marginRight;
                float height = abAnchorBottom - transform.position.y + layout.marginBottom;

                if (width != layout.width || height != layout.height){
                    layout.width = width;
                    layout.height = height;
                    layout.needUpdateSizes = true;
                }

                layout.needUpdateAnchors = false;
            }else{
                layout.marginLeft = transform.position.x - abAnchorLeft;
                layout.marginTop = transform.position.y - abAnchorTop;
                layout.marginRight = layout.width + transform.position.x - abAnchorRight;
                layout.marginBottom = layout.height + transform.position.y - abAnchorBottom;
            }
        }

        if (signature.test(scene->getComponentType<UIContainerComponent>())){
            UIContainerComponent& container = scene->getComponent<UIContainerComponent>(entity);
            // reseting all container boxes
            for (int b = 0; b < MAX_CONTAINER_BOXES; b++){
                container.boxes[b].layout = NULL_ENTITY;
            }
            container.numBoxes = 0;
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
    auto layouts = scene->getComponentArray<UILayoutComponent>();
    for (int i = 0; i < layouts->size(); i++){
        UILayoutComponent& layout = layouts->getComponentFromIndex(i);

        Entity entity = layouts->getEntity(i);
        Signature signature = scene->getSignature(entity);
        if (signature.test(scene->getComponentType<TextEditComponent>()) && signature.test(scene->getComponentType<UIComponent>())){
            TextEditComponent& textedit = scene->getComponent<TextEditComponent>(entity);
            UIComponent& ui = scene->getComponent<UIComponent>(entity);

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

void UISystem::eventOnPointerDown(float x, float y){
    auto layouts = scene->getComponentArray<UILayoutComponent>();
    int lastUI = -1;

    for (int i = 0; i < layouts->size(); i++){
        UILayoutComponent& layout = layouts->getComponentFromIndex(i);

        Entity entity = layouts->getEntity(i);
        Signature signature = scene->getSignature(entity);
        if (signature.test(scene->getComponentType<Transform>()) && signature.test(scene->getComponentType<UIComponent>())){
            Transform& transform = scene->getComponent<Transform>(entity);
            UIComponent& ui = scene->getComponent<UIComponent>(entity);

            if (isCoordInside(x, y, transform, layout)){ //TODO: isCoordInside to polygon
                if (signature.test(scene->getComponentType<ButtonComponent>()) || 
                    signature.test(scene->getComponentType<TextEditComponent>()) ||
                    signature.test(scene->getComponentType<ImageComponent>())){
                    lastUI = i;
                }
            }

            if (ui.focused){
                ui.focused = false;
                ui.onLostFocus.call();
            }
        }
    }

    if (lastUI != -1){
        UILayoutComponent& layout = layouts->getComponentFromIndex(lastUI);
        Entity entity = layouts->getEntity(lastUI);
        Signature signature = scene->getSignature(entity);

        if (signature.test(scene->getComponentType<Transform>()) && signature.test(scene->getComponentType<UIComponent>())){
            Transform& transform = scene->getComponent<Transform>(entity);
            UIComponent& ui = scene->getComponent<UIComponent>(entity);
            
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

            ui.onPointerDown(x - transform.worldPosition.x, y - transform.worldPosition.y);

            if (!ui.focused){
                ui.focused = true;
                ui.onGetFocus.call();
            }
        }
    }else{
        System::instance().hideVirtualKeyboard();
    }
}

void UISystem::eventOnPointerUp(float x, float y){
    auto layouts = scene->getComponentArray<UILayoutComponent>();
    for (int i = 0; i < layouts->size(); i++){
        UILayoutComponent& layout = layouts->getComponentFromIndex(i);

        Entity entity = layouts->getEntity(i);
        Signature signature = scene->getSignature(entity);
        if (signature.test(scene->getComponentType<Transform>()) && signature.test(scene->getComponentType<UIComponent>())){
            Transform& transform = scene->getComponent<Transform>(entity);
            UIComponent& ui = scene->getComponent<UIComponent>(entity);

            if (signature.test(scene->getComponentType<ButtonComponent>())){
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

            ui.onPointerUp(x - transform.worldPosition.x, y - transform.worldPosition.y);
        }
    }
}

void UISystem::eventOnPointerMove(float x, float y){
    auto layouts = scene->getComponentArray<UILayoutComponent>();
    int lastUI = -1;

    for (int i = 0; i < layouts->size(); i++){
        UILayoutComponent& layout = layouts->getComponentFromIndex(i);

        Entity entity = layouts->getEntity(i);
        Signature signature = scene->getSignature(entity);
        if (signature.test(scene->getComponentType<UIComponent>())){
            UIComponent& ui = scene->getComponent<UIComponent>(entity);
            if (signature.test(scene->getComponentType<Transform>())){
                Transform& transform = scene->getComponent<Transform>(entity);

                if ((!ui.pointerMoved) && isCoordInside(x, y, transform, layout)){
                    lastUI = i;
                }
            }

            ui.pointerMoved = false;
        }
    }

    if (lastUI != -1){
        UILayoutComponent& layout = layouts->getComponentFromIndex(lastUI);
        Entity entity = layouts->getEntity(lastUI);
        Signature signature = scene->getSignature(entity);

        if (signature.test(scene->getComponentType<Transform>()) && signature.test(scene->getComponentType<UIComponent>())){
            Transform& transform = scene->getComponent<Transform>(entity);
            UIComponent& ui = scene->getComponent<UIComponent>(entity);

            ui.onPointerMove.call(x - transform.worldPosition.x, y - transform.worldPosition.y);
            ui.pointerMoved = true;
        }
    }
}

bool UISystem::isCoordInside(float x, float y, Transform& transform, UILayoutComponent& layout){
    Vector3 point = transform.worldRotation.getRotationMatrix() * Vector3(x, y, 0);
    Vector2 center = Vector2(0, 0);

    if (point.x >= (transform.worldPosition.x - center.x) &&
        point.x <= (transform.worldPosition.x - center.x + abs(layout.width * transform.worldScale.x)) &&
        point.y >= (transform.worldPosition.y - center.y) &&
        point.y <= (transform.worldPosition.y - center.y + abs(layout.height * transform.worldScale.y))) {
        return true;
    }
    return false;
}