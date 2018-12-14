#include "TextEdit.h"

#include <locale>
#include <codecvt>
#include "platform/SystemPlatform.h"
#include "Engine.h"
#include "Log.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

TextEdit::TextEdit(): UIImage(){
    text.setMultiline(false);

    addObject(&text);
    addObject(&cursor);

    setClipping(true);

    cursorBlinkTimer = 0;
    cursorSize = Vector2(6, 0);
}

TextEdit::~TextEdit(){
}

void TextEdit::adjustText(){
    Vector2 lastCharPos = text.getCharPosition(text.getNumChars() - 1);

    float posX = border_left;
    float posY = (height / 2) + (this->text.getHeight() / 2) - border_bottom;
    if (this->text.getWidth() > getWidth()){
        posX = getWidth() - lastCharPos.x - border_right - cursorSize.x - 1;
    }
    this->text.setPosition(posX, posY);

    if (cursorSize.y != text.getLineHeight()){
        cursorSize.y = text.getLineHeight();

        cursor.clear();
        cursor.addVertex(Vector3(0, 0, 0));
        cursor.addVertex(Vector3(0, cursorSize.y, 0));
        cursor.addVertex(Vector3(cursorSize.x, 0, 0));
        cursor.addVertex(Vector3(cursorSize.x, cursorSize.y, 0));

        cursor.updateBuffers();
    }

    cursor.setPosition(text.getPosition().x + lastCharPos.x, text.getPosition().y - text.getHeight() + text.getLineGap());
    cursor.setColor(text.getColor());
}

void TextEdit::setText(std::string text){
    this->text.setText(text.c_str());

    adjustText();
}

void TextEdit::setTextFont(std::string font){
    this->text.setFont(font.c_str());
}

void TextEdit::setTextSize(unsigned int size){
    this->text.setFontSize(size);
}

void TextEdit::setTextColor(Vector4 color){
    this->text.setColor(color);
}

std::string TextEdit::getText(){
    return this->text.getText();
}

Text* TextEdit::getTextObject(){
    return &text;
}

void TextEdit::getFocus(){
    SystemPlatform::instance().showVirtualKeyboard();
    UIObject::getFocus();
}

void TextEdit::lostFocus(){
    SystemPlatform::instance().hideVirtualKeyboard();
    UIObject::lostFocus();
}

void TextEdit::engineOnTextInput(std::string text){

    if (focused) {

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > convert;

        std::wstring utf16Text = convert.from_bytes(text);
        std::wstring utf16OldText = convert.from_bytes(getText());

        for (int i = 0; i < utf16Text.size(); i++) {
            std::string newText = "";
            if (utf16Text[i] == '\b') {
                newText = convert.to_bytes(utf16OldText.substr(0, utf16OldText.size() - 1));
                setText(newText);
            } else if (utf16Text[i] == '\n') {
                SystemPlatform::instance().hideVirtualKeyboard();
            } else {
                newText = getText() + convert.to_bytes(utf16Text[i]);
                setText(newText);
            }
        }

    }

    UIObject::engineOnTextInput(text);
}

bool TextEdit::load(){

    text.load();

    adjustText();

    return UIImage::load();
}

bool TextEdit::draw(){

    if (focused) {
        cursorBlinkTimer += Engine::getDeltatime();

        if (cursorBlinkTimer > 600) {
            if (cursor.isVisible()) {
                cursor.setVisible(false);
            } else {
                cursor.setVisible(true);
            }
            cursorBlinkTimer = 0;
        }
    }else{
        cursor.setVisible(false);
    }

    return UIImage::draw();
}
