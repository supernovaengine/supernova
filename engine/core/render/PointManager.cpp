#include "PointManager.h"

#include "Supernova.h"
#include "gles2/GLES2Point.h"


PointManager::PointManager() {
    render = NULL;
    instanciateRender();
}

PointManager::~PointManager() {
    delete render;
}

void PointManager::instanciateRender(){
    if (render == NULL){
        if (Supernova::getRenderAPI() == S_GLES2){
            render = new GLES2Point();
        }
    }
}

PointRender* PointManager::getRender(){
    instanciateRender();
    return render;
}

bool PointManager::load() {
    instanciateRender();
    return render->load();
}

bool PointManager::draw() {
    return render->draw();
}

void PointManager::destroy(){
    if (render != NULL)
        render->destroy();
}
