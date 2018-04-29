#include "TextEdit.h"

//#include "SupernovaAndroid.h"
//#include "SupernovaIOS.h"

using namespace Supernova;

TextEdit::TextEdit(): GUIImage(){
    text.setMultiline(false);
    addObject(&text);
}

TextEdit::~TextEdit(){
}

void TextEdit::setText(std::string text){
    this->text.setText(text.c_str());
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

void TextEdit::engine_onPress(float x, float y){
    //SupernovaAndroid::showSoftKeyboard();
    //SupernovaIOS::showSoftKeyboard();
    GUIObject::engine_onPress(x, y);
}

void TextEdit::engine_onUp(float x, float y){
    GUIObject::engine_onUp(x, y);
}

void TextEdit::engine_onTextInput(std::string text){
    std::string newText = text;

    if (text == "\b") {
        newText = getText().substr(0, getText().size()-1);
    }else if (text == "\n") {
        //SupernovaIOS::hideSoftKeyboard();
    }else{
        newText = getText() + text;
    }
    setText(newText);

    GUIObject::engine_onTextInput(text);
}

bool TextEdit::load(){
    
    setClipping(true);

    text.load();

    float labelX = border_left;
    float labelY = (height / 2) + (text.getHeight() / 2) - border_bottom;

    text.setPosition(labelX, labelY, 0);

    return GUIImage::load();
}
