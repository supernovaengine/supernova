
#include "GLES2Scene.h"

#include "GLES2Header.h"
#include "GLES2Util.h"
#include "math/Angle.h"
#include "Engine.h"


using namespace Supernova;

GLES2Scene::GLES2Scene(): SceneRender() {
}

GLES2Scene::~GLES2Scene() {
}


bool GLES2Scene::load() {

    if (!SceneRender::load()){
        return false;
    }

    if (!childScene) {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        GLES2Util::checkGlError("Error on load scene GLES2");
    }

    return true;
}

bool GLES2Scene::draw() {

    if (!SceneRender::draw()){
        return false;
    }

    if (!childScene) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        GLES2Util::checkGlError("glClearColor");

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GLES2Util::checkGlError("glClear");
    }

    if (useDepth){
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
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

bool GLES2Scene::viewSize(Rect rect){
    //Convert top-left orientation to botton-left (OpenGL default)
    glViewport(rect.getX(), Engine::getScreenHeight() - rect.getY() - rect.getHeight(), rect.getWidth(), rect.getHeight());
    GLES2Util::checkGlError("glViewport");
    
    return true;
}

bool GLES2Scene::enableScissor(Rect rect){
    //Convert top-left orientation to botton-left (OpenGL default)
    glScissor(rect.getX(), Engine::getScreenHeight() - rect.getY() - rect.getHeight(), rect.getWidth(), rect.getHeight());
    GLES2Util::checkGlError("glScissor");
    glEnable(GL_SCISSOR_TEST);

    return true;
}

bool GLES2Scene::disableScissor(){
    glDisable(GL_SCISSOR_TEST);

    return true;
}

bool GLES2Scene::isEnabledScissor(){
    return glIsEnabled(GL_SCISSOR_TEST);
}

Rect GLES2Scene::getActiveScissor(){
    int rect[4];
    glGetIntegerv(GL_SCISSOR_BOX, rect);

    return Rect(rect[0], rect[1], rect[2], rect[3]);
}
