// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "TextEdit.h"

#include "component/PolygonComponent.h"
#include "component/TextComponent.h"
#include "component/UIComponent.h"
#include "subsystem/UISystem.h"
#include "util/StringUtils.h"

using namespace doriax;

TextEdit::TextEdit(Scene* scene): Image(scene){
    addComponent<TextEditComponent>();
}

TextEdit::TextEdit(Scene* scene, Entity entity): Image(scene, entity){
}

TextEdit::~TextEdit(){
}

bool TextEdit::hasText() const{
    const TextEditComponent& tecomp = getComponent<TextEditComponent>();
    if (tecomp.text == NULL_ENTITY){
        return false;
    }

    Signature signature = scene->getSignature(tecomp.text);
    return signature.test(scene->getComponentId<TextComponent>());
}

bool TextEdit::hasCursor() const{
    const TextEditComponent& tecomp = getComponent<TextEditComponent>();
    if (tecomp.cursor == NULL_ENTITY){
        return false;
    }

    Signature signature = scene->getSignature(tecomp.cursor);
    return signature.test(scene->getComponentId<PolygonComponent>());
}

bool TextEdit::hasSelection() const{
    const TextEditComponent& tecomp = getComponent<TextEditComponent>();
    if (tecomp.selection == NULL_ENTITY){
        return false;
    }

    Signature signature = scene->getSignature(tecomp.selection);
    return signature.test(scene->getComponentId<PolygonComponent>());
}

void TextEdit::ensureText(){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    if (!hasText() || !hasCursor() || !hasSelection()){
        scene->getSystem<UISystem>()->createTextEditObjects(getEntity(), tecomp);
    }
}

Text TextEdit::getTextObject() const{
    const TextEditComponent& tecomp = getComponent<TextEditComponent>();
    if (!hasText()){
        return Text(scene, NULL_ENTITY);
    }

    return Text(scene, tecomp.text);
}

// Fully qualify Polygon: UISystem.h pulls in wingdi.h on Windows, whose GDI Polygon()
// function clashes with doriax::Polygon when used as an unqualified return type here.
doriax::Polygon TextEdit::getCursorObject() const{
    const TextEditComponent& tecomp = getComponent<TextEditComponent>();
    if (!hasCursor()){
        return Polygon(scene, NULL_ENTITY);
    }

    return Polygon(scene, tecomp.cursor);
}

doriax::Polygon TextEdit::getSelectionObject() const{
    const TextEditComponent& tecomp = getComponent<TextEditComponent>();
    if (!hasSelection()){
        return Polygon(scene, NULL_ENTITY);
    }

    return Polygon(scene, tecomp.selection);
}

void TextEdit::setDisabled(bool disabled){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();

    tecomp.disabled = disabled;

    tecomp.needUpdateTextEdit = true;
}

bool TextEdit::getDisabled() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();

    return  tecomp.disabled;
}

void TextEdit::setText(const std::string& text){
    ensureText();
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(tecomp.text);

    if (textcomp.text != text){
        textcomp.text = text;
        textcomp.needUpdateText = true;
        tecomp.cursorIndex = static_cast<int>(StringUtils::countCodepoints(text));
        tecomp.selectionAnchor = tecomp.cursorIndex;
        tecomp.scrollOffset = 0;
        tecomp.needUpdateTextEdit = true;
    }
}

std::string TextEdit::getText() const{
    if (!hasText()){
        return "";
    }

    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(tecomp.text);

    return textcomp.text;
}

void TextEdit::setTextColor(Vector4 color){
    ensureText();
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    UIComponent& uitext = scene->getComponent<UIComponent>(tecomp.text);

    uitext.color = Color::sRGBToLinear(color);
}

void TextEdit::setTextColor(const float red, const float green, const float blue, const float alpha){
    setTextColor(Vector4(red, green, blue, alpha));
}

void TextEdit::setTextColor(const float red, const float green, const float blue){
    setTextColor(Vector4(red, green, blue, getTextColor().w));
}

Vector4 TextEdit::getTextColor() const{
    if (!hasText()){
        return Vector4(0.0, 0.0, 0.0, 1.0);
    }

    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    UIComponent& uitext = scene->getComponent<UIComponent>(tecomp.text);

    return Color::linearTosRGB(uitext.color);
}

void TextEdit::setTextFont(const std::string& font){
    ensureText();
    TextEditComponent& tecomp = getComponent<TextEditComponent>();

    getTextObject().setFont(font);

    tecomp.needUpdateTextEdit = true;
}

std::string TextEdit::getTextFont() const{
    if (!hasText()){
        return "";
    }

    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    TextComponent& textcomp = scene->getComponent<TextComponent>(tecomp.text);

    return textcomp.font[0];
}

void TextEdit::setFontSize(unsigned int fontSize){
    ensureText();
    TextEditComponent& tecomp = getComponent<TextEditComponent>();

    getTextObject().setFontSize(fontSize);

    tecomp.needUpdateTextEdit = true;
}

unsigned int TextEdit::getFontSize() const{
    if (!hasText()){
        return 0;
    }

    return getTextObject().getFontSize();
}

void TextEdit::setMaxTextSize(unsigned int maxTextSize){
    ensureText();
    getTextObject().setMaxTextSize(maxTextSize);
}

unsigned int TextEdit::getMaxTextSize() const{
    if (!hasText()){
        return 0;
    }

    return getTextObject().getMaxTextSize();
}

void TextEdit::setPlaceholder(const std::string& placeholder){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    if (tecomp.placeholder != placeholder){
        tecomp.placeholder = placeholder;
        tecomp.needUpdateTextEdit = true;
    }
}

std::string TextEdit::getPlaceholder() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    return tecomp.placeholder;
}

void TextEdit::setPlaceholderColor(Vector4 color){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    tecomp.placeholderColor = Color::sRGBToLinear(color);
    tecomp.needUpdateTextEdit = true;
}

void TextEdit::setPlaceholderColor(const float red, const float green, const float blue, const float alpha){
    setPlaceholderColor(Vector4(red, green, blue, alpha));
}

void TextEdit::setPlaceholderColor(const float red, const float green, const float blue){
    setPlaceholderColor(Vector4(red, green, blue, getPlaceholderColor().w));
}

Vector4 TextEdit::getPlaceholderColor() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    return Color::linearTosRGB(tecomp.placeholderColor);
}

void TextEdit::setPassword(bool password){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    if (tecomp.password != password){
        tecomp.password = password;
        tecomp.needUpdateTextEdit = true;
    }
}

bool TextEdit::getPassword() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    return tecomp.password;
}

void TextEdit::setPasswordChar(char passwordChar){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    if (tecomp.passwordChar != passwordChar){
        tecomp.passwordChar = passwordChar;
        tecomp.needUpdateTextEdit = true;
    }
}

char TextEdit::getPasswordChar() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    return tecomp.passwordChar;
}

void TextEdit::setCursorIndex(int cursorIndex){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    tecomp.cursorIndex = cursorIndex;
    tecomp.selectionAnchor = cursorIndex;
    tecomp.needUpdateTextEdit = true;
}

int TextEdit::getCursorIndex() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    return tecomp.cursorIndex;
}

void TextEdit::setSelection(int anchor, int focus){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    tecomp.selectionAnchor = anchor;
    tecomp.cursorIndex = focus;
    tecomp.needUpdateTextEdit = true;
}

int TextEdit::getSelectionAnchor() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    return tecomp.selectionAnchor;
}

int TextEdit::getSelectionFocus() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    return tecomp.cursorIndex;
}

void TextEdit::setCursorBlink(float cursorBlink){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();

    tecomp.cursorBlink = cursorBlink;
}

float TextEdit::getCursorBlink() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();

    return tecomp.cursorBlink;
}

void TextEdit::setCursorWidth(float cursorWidth){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    if (tecomp.cursorWidth != cursorWidth){
        tecomp.cursorWidth = cursorWidth;
        tecomp.needUpdateTextEdit = true;
    }
}

float TextEdit::getCursorWidth() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();

    return tecomp.cursorWidth;
}

void TextEdit::setCursorColor(Vector4 color){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    tecomp.needUpdateTextEdit = true;
    tecomp.cursorColor = Color::sRGBToLinear(color);
}

void TextEdit::setCursorColor(const float red, const float green, const float blue, const float alpha){
    setCursorColor(Vector4(red, green, blue, alpha));
}

void TextEdit::setCursorColor(const float red, const float green, const float blue){
    setCursorColor(Vector4(red, green, blue, getCursorColor().w));
}

Vector4 TextEdit::getCursorColor() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    return Color::linearTosRGB(tecomp.cursorColor);
}

void TextEdit::setSelectionColor(Vector4 color){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    tecomp.selectionColor = Color::sRGBToLinear(color);
    tecomp.needUpdateTextEdit = true;
}

void TextEdit::setSelectionColor(const float red, const float green, const float blue, const float alpha){
    setSelectionColor(Vector4(red, green, blue, alpha));
}

void TextEdit::setSelectionColor(const float red, const float green, const float blue){
    setSelectionColor(Vector4(red, green, blue, getSelectionColor().w));
}

Vector4 TextEdit::getSelectionColor() const{
    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    return Color::linearTosRGB(tecomp.selectionColor);
}
