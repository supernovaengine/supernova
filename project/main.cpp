
#include "Supernova.h"
using namespace Supernova;

#include "Sprite.h"
#include "Camera.h"
#include "Polygon.h"
#include "Input.h"
#include "Angle.h"
#include "SpriteAnimation.h"
#include "PositionAction.h"
#include "RotationAction.h"
#include "ScaleAction.h"
#include "ColorAction.h"
#include "Particles.h"
#include "ParticlesAnimation.h"

Scene scene;
Camera camera(&scene);

Polygon polygonroot(&scene);
Polygon polygon(&scene);
Polygon polygon2(&scene);
Polygon polygon3(&scene);
Sprite sprite(&scene);
SpriteAnimation spriteanim(&scene);
PositionAction positionaction(&scene);
RotationAction rotationaction(&scene);
ScaleAction scaleaction(&scene);
ColorAction coloraction(&scene);
Particles particles(&scene);
ParticlesAnimation partianim(&scene);


void onActionStart();
void onKeyDown(int key, bool repeat, int mods);

void init(){

    camera.activate();
    //camera.setType(CameraType::CAMERA_PERSPECTIVE);

    camera.setPosition(0,8,-100);

    sprite.setTexture("pista.png");
    //sprite.setColor(0.2, 0.0, 0.5, 1.0);
    sprite.addFrame(0, "", Rect(0.5, 0.5, 0.5, 0.5));
    sprite.addFrame(1, "", Rect(0.5, 0.0, 0.5, 0.5));
    sprite.addFrame(2, "", Rect(0.0, 0.5, 0.5, 0.5));
    sprite.setName("Sprite");
    sprite.setPosition(20,20,0);

    spriteanim.setTarget(sprite.getEntity());

    spriteanim.setAnimation({0, 1, 2}, {100, 100, 100}, true);
    spriteanim.getComponent<ActionComponent>().onStart = onActionStart;

    Quaternion startRot;
    Quaternion endRot;
    startRot.fromAngle(Angle::degToDefault(0));
    endRot.fromAngle(Angle::degToDefault(180));
    rotationaction.setAction(startRot, endRot, 5, false);
    rotationaction.setTarget(polygonroot.getEntity());

    scaleaction.setAction(Vector3(1,1,1), Vector3(2,2,2), 10, false);
    scaleaction.setTarget(sprite.getEntity());

    coloraction.setAction(sprite.getColor(), Vector4(0.0, 0.0, 1.0, 1.0), 5, false);
    coloraction.setTarget(sprite.getEntity());

    positionaction.setAction(Vector3(20,20,0), Vector3(200,200,0), 5, false);
    positionaction.setTarget(sprite.getEntity());
    positionaction.setFunctionType(S_EASE_ELASTIC_IN_OUT);

    polygonroot.addVertex(0, 0);
    polygonroot.addVertex(30, 0);
    polygonroot.addVertex(0, 30);
    polygonroot.addVertex(30, 30);
    polygonroot.setColor(0.8, 0.8, 0.5, 1.0);
    polygonroot.setPosition(0, 0, 0);
    polygonroot.setName("PolygonRoot");

    polygonroot.addChild(&polygon);
    polygonroot.addChild(&polygon2);
    polygonroot.addChild(&polygon3);
    polygonroot.addChild(&sprite);

    polygon.addVertex(0, 0);
    polygon.addVertex(300, 0);
    polygon.addVertex(0, 300);
    polygon.addVertex(300, 300);
    polygon.setColor(1.0, 0.3, 0.8, 1.0);
    polygon.setPosition(50, 50, 0);
    polygon.setName("Polygon1");

    polygon2.addVertex(0, 0);
    polygon2.addVertex(300, 0);
    polygon2.addVertex(0, 300);
    polygon2.addVertex(300, 300);
    polygon2.setColor(1.0, 0.5, 1.0, 1.0);
    polygon2.setPosition(70, 70, 0);
    polygon2.setName("Polygon2");

    polygon3.addVertex(0, 0);
    polygon3.addVertex(300, 0);
    polygon3.addVertex(0, 300);
    polygon3.addVertex(300, 300);
    polygon3.setColor(0.5, 1.0, 1.0, 1.0);
    polygon3.setPosition(100, 100, 0);
    polygon3.setName("Polygon3");
    polygon3.setTexture("pista.png");

    particles.setPosition(300, 100, 0);
    particles.addParticle(Vector3(30, 30, 0), Vector4(1.0, 1.0, 1.0, 1.0), 50, 40);
    particles.addParticle(Vector3(20, 60, 0), Vector4(0.0, 1.0, 0.0, 1.0), 40, 0, Rect(0, 0, 0.5, 0.5));
    particles.setTexture("pista.png");

    particles.addSpriteFrame(0, "", Rect(0.5, 0.5, 0.5, 0.5));
    particles.addSpriteFrame(1, "", Rect(0.5, 0.0, 0.5, 0.5));
    particles.addSpriteFrame(2, "", Rect(0.0, 0.5, 0.5, 0.5));

    partianim.setTarget(particles.getEntity());
    partianim.setLifeInitializer(10);
    partianim.setVelocityInitializer(Vector3(0,10,0), Vector3(0,50,0));
    partianim.setVelocityModifier(5, 8, Vector3(0,10,0), Vector3(0,300,0), S_EASE_CUBIC_IN_OUT);
    partianim.setSizeInitializer(10, 50);
    partianim.setSpriteIntializer(0, 2);
    //partianim.setSpriteModifier(5, 8, {0,1,2});
    //partianim.setRotationInitializer(90);
    partianim.setRotationModifier(1, 5, 0, 360);

    //plane.create(4000, 4000);
    //plane.setColor(0.5, 0.3, 0.7, 1.0);
    //plane.setTexture("pista.png");
    //plane.setPosition(-2000,0,-2000);

    Engine::setScene(&scene);
    Engine::onKeyDown = onKeyDown;
}

void onActionStart(){
    Log::Verbose("Action Start");
}

void onKeyDown(int key, bool repeat, int mods){
    if (key == S_KEY_A){
        sprite.moveToLast();
    }
    if (key == S_KEY_F){
        sprite.moveToFirst();
    }
    if (key == S_KEY_S){
        sprite.moveDown();
    }
    if (key == S_KEY_D){
        sprite.moveUp();
    }

    if (key == S_KEY_1){
        sprite.setFrame(0);
    }
    if (key == S_KEY_2){
        sprite.setFrame(1);
    }
    if (key == S_KEY_3){
        sprite.setFrame(2);
    }

    if (key == S_KEY_R){
        //sprite.startAnimation({0, 1, 2}, {1000, 1000, 1000}, true);
        spriteanim.start();
        positionaction.start();
        //rotationaction.start();
        //scaleaction.start();
        coloraction.start();
        partianim.start();
    }
    if (key == S_KEY_T){
        spriteanim.pause();
        //sprite.pauseAnimation();
    }
    if (key == S_KEY_Y){
        spriteanim.stop();
        //sprite.stopAnimation();
    }
}


/*
#include "Supernova.h"
#include "Mesh.h"
#include "Object.h"
#include "Camera.h"
#include "System.h"
#include "util/StringUtils.h"
#include "Input.h"
#include "Model.h"
#include "PlaneTerrain.h"
#include "SkyBox.h"
#include "Light.h"
#include "math/Angle.h"

using namespace Supernova;

void onUpdate();
void onCharInput(wchar_t);
void onViewLoaded();
void onKeyDown(int key, bool repeat, int mods);
void onKeyUp(int key, bool repeat, int mods);
void onMouseDown(int button, int mods);
void onMouseUp(int button, int mods);
void onMouseMove(float x, float y);
void onMouseScroll(float xoffset, float yoffset);
void onMouseEnter();
void onMouseLeave();
void onTouchStart(int pointer, float x, float y);
void onTouchMove(int pointer, float x, float y);
void onTouchEnd(int pointer, float x, float y);
void onTouchCancel();

Scene scene;
//Mesh child2(&scene);
//Object objeto(&scene);
//Mesh child1(&scene);
Model carro(&scene);
//Mesh chefao(&scene);
//Mesh* child11;
PlaneTerrain plane(&scene);
SkyBox sky(&scene);
Light lightDir(&scene);
//Light lightDir2(&scene);
Light lightPoint(&scene);
Light lightSpot(&scene);

float rotationY = 0;
float speed = 0;

Camera camera(&scene);

void init(){

    lightDir.setType(LightType::DIRECTIONAL);
    lightDir.setDirection(0.5f, -0.5, 0.0);
    lightDir.setRange(0.0);
    lightDir.setPosition(0.0f, 0.0f, 0.0f);
    lightDir.setIntensity(1.0);
    lightDir.setShadows(true);

    //lightDir2.setType(LightType::DIRECTIONAL);
    //lightDir2.setDirection(-0.5f, -0.5, 0.0);
    //lightDir2.setRange(0.0);
    //lightDir2.setPosition(0.0f, 0.0f, 0.0f);
    //lightDir2.setIntensity(1.0);
    //lightDir2.setShadows(false);

    lightPoint.setType(LightType::POINT);
    lightPoint.setDirection(0.0, 0.0, 0.0);
    lightPoint.setRange(0.0);
    lightPoint.setPosition(300.0f, 80.0f, 80.0f);
    lightPoint.setIntensity(100000.0);
    lightPoint.setShadows(true);

    lightSpot.setType(LightType::SPOT);
    lightSpot.setDirection(0.0f, -0.7, 0.3);
    lightSpot.setRange(0.0);
    lightSpot.setPosition(0.0f, 150.0f/5.0, 0.0f);
    lightSpot.setIntensity(100000.0);
    lightSpot.setConeAngle(50, 70);
    lightSpot.setShadows(true);
    carro.addChild(&lightSpot);

    sky.setTextureLeft("ely_hills/hills_lf.tga");
    sky.setTextureRight("ely_hills/hills_rt.tga");
    sky.setTextureBack("ely_hills/hills_bk.tga");
    sky.setTextureFront("ely_hills/hills_ft.tga");
    sky.setTextureUp("ely_hills/hills_up.tga");
    sky.setTextureDown("ely_hills/hills_dn.tga");

    //camera.setOrtho(-500, 500, -500, 500, 1, 2000);
    camera.setType(CameraType::CAMERA_PERSPECTIVE);
    camera.activate();

    camera.setPosition(0,80,100);
    //carro.addChild(&camera);

    carro.loadOBJ("jeep/Jeep.obj");
    carro.setPosition(0, 0, 20);
    carro.setScale(5);

    //carro.loadGLTF("WaterBottle.glb");
    //carro.setPosition(0, 30, 20);
    //carro.setScale(200);

    carro.setName("carro");

    plane.create(4000, 4000);
    //plane.setColor(0.5, 0.3, 0.7, 1.0);
    plane.setTexture("pista.png");
    plane.setPosition(-2000,0,-2000);

   
//    std::vector<std::string> teste = System::instance().args;
//
//    child2.addVertex(0.0f,  1.0f);
//    child2.addVertex(0.5f,  -1.0f);
//    child2.addVertex(-0.5f,  -1.0f);
//    child2.setName("child2");
// 
//    child11= new Mesh(&scene);
//    child11->addVertex(0.0f,  1.0f);
//    child11->addVertex(0.5f,  -1.0f);
//    child11->addVertex(-0.5f,  -1.0f);
//    child11->setName("child11");
//
//    chefao.addVertex(0.0f,  1.0f);
//    chefao.addVertex(0.5f,  -0.5f);
//    chefao.addVertex(-0.5f,  -0.5f);
//    chefao.setName("chefao");
//
//    //objeto.addChild(&child1);
//    chefao.addChild(&child1);
//    chefao.addChild(&child2);
//    child1.addChild(child11);
//    objeto.addChild(&chefao);
//
//    std::vector<int> vetor;
//    vetor.push_back(0);
//    vetor.push_back(1);
//    vetor.push_back(2);
//    vetor.push_back(0);
//    vetor.push_back(0);
//    vetor.push_back(3);
//    vetor.push_back(4);
//    vetor.push_back(5);
//    vetor.push_back(6);
//    vetor.push_back(7);
//
//    move_range<int>(3, 2, 6, vetor);

    //Engine::setScalingMode(Scaling::LETTERBOX);
    
    Engine::setAllowEventsOutCanvas(true);
    
    Engine::setCallMouseInTouchEvent(true);
    Engine::setCallTouchInMouseEvent(true);

    Engine::onUpdate = onUpdate;
    Engine::onCharInput = onCharInput;
    Engine::onViewLoaded = onViewLoaded;
    Engine::onKeyDown = onKeyDown;
    Engine::onKeyUp = onKeyUp;
    Engine::onMouseDown = onMouseDown;
    Engine::onMouseUp = onMouseUp;
    Engine::onKeyDown = onKeyDown;
    Engine::onMouseMove = onMouseMove;
    Engine::onMouseScroll = onMouseScroll;
    Engine::onMouseEnter = onMouseEnter;
    Engine::onMouseLeave = onMouseLeave;
    Engine::onTouchStart = onTouchStart;
    Engine::onTouchMove = onTouchMove;
    Engine::onTouchEnd = onTouchEnd;
    Engine::onTouchCancel = onTouchCancel;

    Engine::setScene(&scene);
}

void onUpdate(){
    if (Input::isKeyPressed(S_KEY_LEFT)){
        rotationY += 4;
    }
    if (Input::isKeyPressed(S_KEY_RIGHT)){
        rotationY -= 4;
    }

    if (rotationY > 360) rotationY = rotationY - 360;
    if (rotationY < 0) rotationY = 360 + rotationY;

    
    if (speed >= 0.3){
         speed -= 0.3;
    }else if (speed <= -0.3){
        speed += 0.3;
    }else{
        speed = 0;
    }
    if (Input::isKeyPressed(S_KEY_UP)){
        if (speed < 10)
            speed += 1;
    }
    if (Input::isKeyPressed(S_KEY_DOWN)){
        if (speed > -10)
            speed -= 1;
    }

    Vector3 vDirection(cos(Angle::degToRad(rotationY-90)), 0, -sin(Angle::degToRad(rotationY-90)));

    carro.setPosition(carro.getPosition() + (vDirection*speed));
    carro.setRotation(0, rotationY, 0);
    //camera.walkForward(0.5);

    camera.setView(carro.getPosition());
}

void onViewLoaded(){
    //System::instance().showVirtualKeyboard();
}

void onTouchStart(int pointer, float x, float y){
    Log::Verbose("Touch start %i - %f %f", pointer, x, y);
}
void onTouchMove(int pointer, float x, float y){
    Log::Verbose("Touch move %i - %f %f", pointer, x, y);
}
void onTouchEnd(int pointer, float x, float y){
    Log::Verbose("Touch end %i - %f %f", pointer, x, y);
}
void onTouchCancel(){
    Log::Verbose("Touch cancel");
}

void onMouseDown(int button, int mods){
    std::string modifier = "";
    if (mods & S_MODIFIER_SHIFT)
        modifier += "shift;";
    if (mods & S_MODIFIER_CONTROL)
        modifier += "control;";
    if (mods & S_MODIFIER_ALT)
        modifier += "alt;";
    if (mods & S_MODIFIER_SUPER)
        modifier += "super;";
    if (mods & S_MODIFIER_CAPS_LOCK)
        modifier += "caps;";
    if (mods & S_MODIFIER_NUM_LOCK)
        modifier += "num;";

    std::string bt = "";
    if (button == S_MOUSE_BUTTON_LEFT)
        bt = "left";
    if (button == S_MOUSE_BUTTON_RIGHT)
        bt = "right";
    if (button == S_MOUSE_BUTTON_MIDDLE)
        bt = "middle";


    Log::Verbose("Mouse down - %s - %s", bt.c_str(), modifier.c_str());
}

void onMouseUp(int button, int mods){
    std::string modifier = "";
    if (mods & S_MODIFIER_SHIFT)
        modifier += "shift;";
    if (mods & S_MODIFIER_CONTROL)
        modifier += "control;";
    if (mods & S_MODIFIER_ALT)
        modifier += "alt;";
    if (mods & S_MODIFIER_SUPER)
        modifier += "super;";
    if (mods & S_MODIFIER_CAPS_LOCK)
        modifier += "caps;";
    if (mods & S_MODIFIER_NUM_LOCK)
        modifier += "num;";

    std::string bt = "";
    if (button == S_MOUSE_BUTTON_LEFT)
        bt = "left";
    if (button == S_MOUSE_BUTTON_RIGHT)
        bt = "right";
    if (button == S_MOUSE_BUTTON_MIDDLE)
        bt = "middle";


    Log::Verbose("Mouse up - %s - %s", bt.c_str(), modifier.c_str());
}

void onMouseMove(float x, float y){
    //Log::Verbose("Mouse %f %f", x, y);
}

void onMouseScroll(float xoffset, float yoffset){
    if (xoffset != 0 || yoffset != 0)
        Log::Verbose("Mouse scroll %f %f", xoffset, yoffset);
}

void onMouseEnter(){
    //Log::Verbose("Mouse enter");
}

void onMouseLeave(){
    //Log::Verbose("Mouse leave");
}

void onCharInput(wchar_t codepoint){
    Log::Verbose("%s",StringUtils::toUTF8(codepoint).c_str());
}

void onKeyDown(int key, bool repeat, int mods){
    std::string modifier = "";
    if (mods & S_MODIFIER_SHIFT)
        modifier += "shift;";
    if (mods & S_MODIFIER_CONTROL)
        modifier += "control;";
    if (mods & S_MODIFIER_ALT)
        modifier += "alt;";
    if (mods & S_MODIFIER_SUPER)
        modifier += "super;";
    if (mods & S_MODIFIER_CAPS_LOCK)
        modifier += "caps;";
    if (mods & S_MODIFIER_NUM_LOCK)
        modifier += "num;";
    
    std::string rstr;
    if (repeat)
        rstr = "repeated";
    
    Log::Verbose("KeyDown: %i - %s - %s",key, modifier.c_str(), rstr.c_str());
}

void onKeyUp(int key, bool repeat, int mods){
    if (key == S_KEY_F){
        if (!System::instance().isFullscreen())
            System::instance().requestFullscreen();
        else
            System::instance().exitFullscreen();
    }

    std::string modifier = "";
    if (mods & S_MODIFIER_SHIFT)
        modifier += "shift;";
    if (mods & S_MODIFIER_CONTROL)
        modifier += "control;";
    if (mods & S_MODIFIER_ALT)
        modifier += "alt;";
    if (mods & S_MODIFIER_SUPER)
        modifier += "super;";
    if (mods & S_MODIFIER_CAPS_LOCK)
        modifier += "caps;";
    if (mods & S_MODIFIER_NUM_LOCK)
        modifier += "num;";
    
    std::string rstr;
    if (repeat)
        rstr = "repeated";
    
    Log::Verbose("KeyUp: %i - %s - %s",key, modifier.c_str(), rstr.c_str());
}
*/