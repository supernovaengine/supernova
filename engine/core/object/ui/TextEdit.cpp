#include "TextEdit.h"

#include "component/TextComponent.h"
#include "subsystem/UISystem.h"

using namespace Supernova;

TextEdit::TextEdit(Scene* scene): Image(scene){
    addComponent<TextEditComponent>({});

    TextEditComponent& tecomp = getComponent<TextEditComponent>();
    //scene->getSystem<UISystem>()->createTextEditObjects(entity, btcomp);
}

void TextEdit::setDisabled(bool disabled){
    TextEditComponent& tecomp = getComponent<TextEditComponent>();

    tecomp.disabled = disabled;

    tecomp.needUpdateTextEdit = true;
}