#include "Supernova.h"
using namespace Supernova;

#include "Image.h"

Scene scene;
Image image(&scene);

SUPERNOVA_INIT void init(){
    image.setAnchorPreset(AnchorPreset::CENTER);
    image.setTexture("supernova.png");

    Engine::setScalingMode(Scaling::FITWIDTH);
    Engine::setCanvasSize(1000,480);
    Engine::setScene(&scene);
}
