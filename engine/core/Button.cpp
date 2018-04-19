
#include "Button.h"

using namespace Supernova;

Button::Button(): GUIImage(){
}

Button::~Button(){
}

void Button::call_onPress(){
    GUIObject::call_onPress();
}
