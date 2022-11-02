#include "Supernova.h"
using namespace Supernova;

#include "Image.h"

Scene scene;
Image image(&scene);

void init(){

    image.setPosition(Vector3(200,200,0));
    image.setTexture("supernova.png");

    Engine::setScalingMode(Scaling::FITWIDTH);
    Engine::setCanvasSize(1000,480);
    Engine::setScene(&scene);
}