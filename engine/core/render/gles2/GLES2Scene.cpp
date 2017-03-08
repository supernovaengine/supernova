
#include "GLES2Scene.h"

#include "GLES2Header.h"
#include "GLES2Util.h"
#include "render/ProgramManager.h"
#include "render/TextureManager.h"
#include "math/Angle.h"


GLES2Scene::GLES2Scene(): SceneRender() {
}

GLES2Scene::~GLES2Scene() {
}


bool GLES2Scene::load(bool childScene) {
    if (!childScene) {
        //ProgramManager::clear();
        //TextureManager::clear();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        GLES2Util::checkGlError("Error on load scene GLES2");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    return true;
}

bool GLES2Scene::draw(bool childScene, bool useDepth, bool useTransparency) {
    if (!childScene) {
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        GLES2Util::checkGlError("glClearColor");

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GLES2Util::checkGlError("glClear");
    }

    if (useDepth){
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }else{
        glDisable(GL_DEPTH_TEST);
    }

    if (useTransparency){
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }else{
        glDisable(GL_BLEND);
    }

    return true;
}

bool GLES2Scene::viewSize(int x, int y, int width, int height){
    
    glViewport(x, y, width, height);
    //glScissor(x, y, width, height);
    //glEnable(GL_SCISSOR_TEST);
    //checkGlError("glViewport");
    
    return true;
}
