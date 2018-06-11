
#include "Button.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

Button::Button(): GUIImage(){
    ownedTextureNormal = true;
    ownedTexturePressed = true;
    ownedTextureDisabled = true;

    down = false;
    pointerDown = -1;

    label.setMultiline(false);
    
    state = S_BUTTON_NORMAL;
}

Button::~Button(){
    if (textureNormal and ownedTextureNormal)
        delete textureNormal;
    
    if (texturePressed and ownedTexturePressed)
        delete texturePressed;
    
    if (textureDisabled and ownedTextureDisabled)
        delete textureDisabled;
}

void Button::setLabelText(std::string text){
    label.setText(text.c_str());
}

void Button::setLabelFont(std::string font){
    if (font != "") {
        label.setFont(font.c_str());
        addObject(&label);
    }
}

void Button::setLabelSize(unsigned int size){
    label.setFontSize(size);
}

void Button::setLabelColor(Vector4 color){
    label.setColor(color);
}

Text* Button::getLabel(){
    return &label;
}

void Button::setTextureNormal(Texture* texture){
    if (textureNormal and ownedTextureNormal)
        delete textureNormal;
    
    textureNormal = texture;
    ownedTextureNormal = false;
    
    setTexture(textureNormal);
}

void Button::setTextureNormal(std::string texturepath){
    if (textureNormal and ownedTextureNormal)
        delete textureNormal;
    
    textureNormal = new Texture(texturepath);
    ownedTextureNormal = true;
    
    setTexture(textureNormal);
}

std::string Button::getTextureNormal(){
    if (textureNormal)
        return textureNormal->getId();
    else
        return NULL;
}

void Button::setTexturePressed(Texture* texture){
    if (texturePressed and ownedTexturePressed)
        delete texturePressed;
    
    texturePressed = texture;
    ownedTexturePressed = false;
}

void Button::setTexturePressed(std::string texturepath){
    if (texturePressed and ownedTexturePressed)
        delete texturePressed;
    
    texturePressed = new Texture(texturepath);
    ownedTexturePressed = true;
}

std::string Button::getTexturePressed(){
    if (texturePressed)
        return texturePressed->getId();
    else
        return NULL;
}

void Button::setTextureDisabled(Texture* texture){
    if (textureDisabled and ownedTextureDisabled)
        delete textureDisabled;
    
    textureDisabled = texture;
    ownedTextureDisabled = false;
}

void Button::setTextureDisabled(std::string texturepath){
    if (textureDisabled and ownedTextureDisabled)
        delete textureDisabled;
    
    textureDisabled = new Texture(texturepath);
    ownedTexturePressed = true;
}

std::string Button::getTextureDisabled(){
    if (textureDisabled)
        return textureDisabled->getId();
    else
        return NULL;
}

void Button::engineOnDown(int pointer, float x, float y){
    if (isCoordInside(x, y)) {
        state = S_BUTTON_PRESSED;
        if (texturePressed) {
            setTexture(texturePressed);
        }
        onDown.call();
        down = true;
        pointerDown = pointer;
    }
    GUIObject::engineOnDown(pointer, x, y);
}

void Button::engineOnUp(int pointer, float x, float y){
    if (pointerDown == pointer) {
        if (state == S_BUTTON_PRESSED) {
            state = S_BUTTON_NORMAL;
            if (textureNormal && texturePressed) {
                setTexture(textureNormal);
            }
            onUp.call();
        }
        down = false;
        pointerDown = -1;
    }
    GUIObject::engineOnUp(pointer, x, y);
}

bool Button::isDown(){
    return down;
}

bool Button::load(){
    if (textureNormal) {
        textureNormal->load();
    }else{
        textureNormal = new Texture(getMaterial()->getTexture()->getId());
        ownedTextureNormal = false;
    }
    
    if (texturePressed)
        texturePressed->load();
    
    if (textureDisabled)
        textureDisabled->load();

    if (!label.getFont().empty()) {
        label.load();

        float labelX = 0;
        float labelY = (height / 2) + (label.getHeight() / 2) - border_bottom;

        if (label.getWidth() > (width - border_right)) {
            labelX = border_left;
            label.setWidth(width - border_right);
        } else {
            labelX = (width / 2) - (label.getWidth() / 2);
        }
        label.setPosition(labelX, labelY, 0);
    }

    return GUIImage::load();
}
