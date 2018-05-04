#include "TextEdit.h"

#include <locale>
#include <codecvt>
#include "platform/SystemPlatform.h"

using namespace Supernova;

TextEdit::TextEdit(): GUIImage(){
    text.setMultiline(false);
    addObject(&text);
}

TextEdit::~TextEdit(){
}

void TextEdit::adjustText(){
    float posX = border_left;
    float posY = (height / 2) + (this->text.getHeight() / 2) - border_bottom;
    if (this->text.getWidth() > getWidth()){
        posX = getWidth() - this->text.getWidth();
    }
    this->text.setPosition(posX, posY);
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

void TextEdit::engine_onDown(float x, float y){
    SystemPlatform::instance().showVirtualKeyboard();
    GUIObject::engine_onDown(x, y);
}

void TextEdit::engine_onUp(float x, float y){
    GUIObject::engine_onUp(x, y);
}

void TextEdit::engine_onTextInput(std::string text){

    std::wstring_convert< std::codecvt_utf8_utf16<wchar_t> > convert;

    std::wstring utf16Text = convert.from_bytes( text );
    std::wstring utf16OldText = convert.from_bytes( getText() );

    for (int i = 0; i < utf16Text.size(); i++){
        std::string newText = "";
        if (utf16Text[i] == '\b') {
            newText = convert.to_bytes(utf16OldText.substr(0, utf16OldText.size()-1));
            setText(newText);
        }else if (utf16Text[i] == '\n') {
            SystemPlatform::instance().hideVirtualKeyboard();
        }else{
            newText = getText() + convert.to_bytes(utf16Text[i]);
            setText(newText);
        }
    }

    GUIObject::engine_onTextInput(text);
}

bool TextEdit::load(){
    
    setClipping(true);

    text.load();

    adjustText();

    return GUIImage::load();
}
