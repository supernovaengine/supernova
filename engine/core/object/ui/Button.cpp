#include "Button.h"

#include "subsystem/UISystem.h"
using namespace Supernova;

Button::Button(Scene* scene): Image(scene){
    addComponent<ButtonComponent>({});
}