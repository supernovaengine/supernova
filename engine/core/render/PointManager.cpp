#include "PointManager.h"

#include "Engine.h"
#include "gles2/GLES2Point.h"

using namespace Supernova;

PointManager::PointManager() {
    render = NULL;
    instanciateRender();
}

PointManager::~PointManager() {
    delete render;
}

void PointManager::instanciateRender(){
    if (render == NULL){
        if (Engine::getRenderAPI() == S_GLES2){
            render = new GLES2Point();
        }
    }
}

PointRender* PointManager::getRender(){
    instanciateRender();
    return render;
}
