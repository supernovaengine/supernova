
#include "Button.h"

using namespace Supernova;

Button::Button(): GUIImage(){
    ownedTextureNormal = true;
    ownedTexturePressed = true;
    ownedTextureDisabled = true;
}

Button::~Button(){
    if (textureNormal and ownedTextureNormal)
        delete textureNormal;
    
    if (texturePressed and ownedTexturePressed)
        delete texturePressed;
    
    if (textureDisabled and ownedTextureDisabled)
        delete textureDisabled;
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

void Button::call_onPress(){
    if (texturePressed){
        setTexture(texturePressed);
    }
    GUIObject::call_onPress();
}

void Button::call_onUp(){
    if (textureNormal){
        setTexture(textureNormal);
    }
    GUIObject::call_onUp();
}

bool Button::load(){
    if (textureNormal)
        textureNormal->load();
    
    if (texturePressed)
        texturePressed->load();
    
    if (textureDisabled)
        textureDisabled->load();

    return GUIImage::load();
}
