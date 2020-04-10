
#include "GLES2Scene.h"

#include "GLES2Header.h"
#include "GLES2Util.h"
#include "math/Angle.h"
#include "Engine.h"
#include "Log.h"


using namespace Supernova;

GLES2Scene::GLES2Scene(): SceneRender() {
/*
    GLint redBits = 0;
    GLint greenBits = 0;
    GLint blueBits = 0;
    GLint alphaBits = 0;
    GLint stencilBits = 0;
    GLint depthBits = 0;
    glGetIntegerv(GL_RED_BITS, &redBits);
    glGetIntegerv(GL_GREEN_BITS, &greenBits);
    glGetIntegerv(GL_BLUE_BITS, &blueBits);
    glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
    glGetIntegerv(GL_STENCIL_BITS, &stencilBits);
    glGetIntegerv(GL_DEPTH_BITS, &depthBits);
    Log::Debug("Red bits: %i", redBits);
    Log::Debug("Green bits: %i", greenBits);
    Log::Debug("Blue bits: %i", blueBits);
    Log::Debug("Alpha bits: %i", alphaBits);
    Log::Debug("Stencil bits: %i", stencilBits);
    Log::Debug("Depth bits: %i", depthBits);
*/
}

GLES2Scene::~GLES2Scene() {
}


bool GLES2Scene::load() {

    if (!SceneRender::load()){
        return false;
    }

    GLES2Util::checkGlError("Error on load scene GLES2");

    return true;
}

bool GLES2Scene::clear(float value) {
    glClearColor(value, value, value, 1.0f);
    GLES2Util::checkGlError("glClearColor");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLES2Util::checkGlError("glClear");

    return true;
}

bool GLES2Scene::draw() {

    if (!SceneRender::draw()){
        return false;
    }
    
    if (drawingShadow)
        glCullFace(GL_FRONT);

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

    glViewport(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());

    GLES2Util::checkGlError("glViewport");
    
    return true;
}

bool GLES2Scene::enableScissor(Rect rect){

    glScissor(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());

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
