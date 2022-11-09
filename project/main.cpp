#include "Supernova.h"
using namespace Supernova;

#include "Image.h"

Scene scene;
Image image(&scene);

void init(){

    image.setAnchors(0.5,0.5,0.5,0.5);
    image.setMargins(-205,-130,205,130);
    image.setTexture("supernova.png");

    Engine::setScalingMode(Scaling::FITWIDTH);
    Engine::setCanvasSize(1000,480);
    Engine::setScene(&scene);
}