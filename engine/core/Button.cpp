
#include "Button.h"


Button::Button(): GUIObject(){
}

Button::~Button(){
    
}

void GUIObject::onTouchPress(float x, float y){
    printf("Encontrou button");
}
