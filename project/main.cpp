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
Light lightPoint(&scene);
Light lightSpot(&scene);

float rotationY = 0;
float speed = 0;

Camera camera(&scene);

void init(){

    lightDir.setType(LightType::DIRECTIONAL);
    lightDir.setDirection(0.8f, -0.2, 0.0);
    lightDir.setRange(0.0);
    lightDir.setPosition(0.0f, 0.0f, 0.0f);
    lightDir.setIntensity(1.0);

    lightPoint.setType(LightType::POINT);
    lightPoint.setDirection(0.0, 0.0, 0.0);
    lightPoint.setRange(0.0);
    lightPoint.setPosition(300.0f, 30.0f, 80.0f);
    lightPoint.setIntensity(10000.0);

    lightSpot.setType(LightType::SPOT);
    lightSpot.setDirection(0.0f, -0.6, 0.4);
    lightSpot.setRange(0.0);
    lightSpot.setPosition(0.0f, 80.0f/5, 30.0f/5);
    lightSpot.setIntensity(10000.0);
    lightSpot.setConeAngle(25, 35);

    carro.addChild(&lightSpot);

    sky.setTextureLeft("ely_hills/hills_lf.tga");
    sky.setTextureRight("ely_hills/hills_rt.tga");
    sky.setTextureBack("ely_hills/hills_bk.tga");
    sky.setTextureFront("ely_hills/hills_ft.tga");
    sky.setTextureUp("ely_hills/hills_up.tga");
    sky.setTextureDown("ely_hills/hills_dn.tga");

    //camera.setOrtho(0, 500, 0, 500, -10, 10);
    camera.activate();

    camera.setPosition(0,30,100);
    //camera.getComponent<CameraComponent>().view = Vector3(0,0,0);

    carro.loadOBJ("jeep/Jeep.obj");
    carro.setPosition(0, 0, 20);
    carro.setScale(5);

    //carro.loadGLTF("WaterBottle.glb");
    //carro.setPosition(0, 30, 20);
    //carro.setScale(200);

    carro.setName("carro");
 /*
    child1.addVertex(-50.0f,  -50.0f);
    child1.addVertex(50.0f,  -50.0f);
    child1.addVertex(-50.0f,  50.0f);
    child1.addVertex(-50.0f,  50.0f);
    child1.addVertex(50.0f,  -50.0f);
    child1.addVertex(50.0f,  50.0f);
    child1.generateTexcoords();
    child1.setTexture("pista.png");
    child1.setName("child1");
*/

    plane.create(1000, 1000);
    //plane.setColor(0.5, 0.3, 0.7, 1.0);
    plane.setTexture("pista.png");
    plane.setPosition(-500,0,-500);

/*   
    std::vector<std::string> teste = System::instance().args;

    child2.addVertex(0.0f,  1.0f);
    child2.addVertex(0.5f,  -1.0f);
    child2.addVertex(-0.5f,  -1.0f);
    child2.setName("child2");
    */
    /*
    child11= new Mesh(&scene);
    child11->addVertex(0.0f,  1.0f);
    child11->addVertex(0.5f,  -1.0f);
    child11->addVertex(-0.5f,  -1.0f);
    child11->setName("child11");

    chefao.addVertex(0.0f,  1.0f);
    chefao.addVertex(0.5f,  -0.5f);
    chefao.addVertex(-0.5f,  -0.5f);
    chefao.setName("chefao");
    */
/*
    //objeto.addChild(&child1);
    chefao.addChild(&child1);
    chefao.addChild(&child2);
    child1.addChild(child11);
    objeto.addChild(&chefao);
*/

/*
    std::vector<int> vetor;
    vetor.push_back(0);
    vetor.push_back(1);
    vetor.push_back(2);
    vetor.push_back(0);
    vetor.push_back(0);
    vetor.push_back(3);
    vetor.push_back(4);
    vetor.push_back(5);
    vetor.push_back(6);
    vetor.push_back(7);

    move_range<int>(3, 2, 6, vetor);
*/
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
