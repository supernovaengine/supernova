#include "DrawManager.h"

#include "Supernova.h"
#include "gles2/GLES2Draw.h"
#include <stddef.h>


DrawManager::DrawManager() {
    render = NULL;
    instanciateRender();
}

DrawManager::~DrawManager() {
    delete render;
}

void DrawManager::instanciateRender(){
    if (render == NULL){
        if (Supernova::getRenderAPI() == S_GLES2){
            render = new GLES2Draw();
        }
    }
}

DrawRender* DrawManager::getRender(){
    instanciateRender();
    return render;
}

bool DrawManager::load() {
    instanciateRender();
    return render->load();
}

bool DrawManager::draw() {
    return render->draw();
}

void DrawManager::destroy(){
    if (render != NULL)
        render->destroy();
}
