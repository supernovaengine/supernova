//
// (c) 2021 Eduardo Doria.
//

#include "ColorAction.h"
#include "math/Color.h"

using namespace Supernova;

ColorAction::ColorAction(Scene* scene): TimedAction(scene){
    addComponent<ColorActionComponent>({});
}

void ColorAction::setAction(Vector4 startColor, Vector4 endColor, float duration, bool loop){
    TimedAction::setAction(duration, loop);

    ColorActionComponent& colorAction = getComponent<ColorActionComponent>();

    colorAction.startColor = startColor;
    colorAction.endColor = endColor;
}