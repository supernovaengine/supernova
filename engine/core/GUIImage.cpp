
#include "GUIImage.h"
#include "PrimitiveMode.h"
#include <string>
#include "image/TextureLoader.h"
#include "render/TextureManager.h"

using namespace Supernova;

GUIImage::GUIImage(): Mesh2D(){
}

GUIImage::~GUIImage(){

}

bool GUIImage::load(){

    return Mesh2D::load();
}
