
#include "GUIImage.h"
#include "PrimitiveMode.h"
#include <string>
#include "image/TextureLoader.h"
#include "render/TextureManager.h"

GUIImage::GUIImage(): Mesh2D(){
}

GUIImage::~GUIImage(){

}

bool GUIImage::load(){

    if (submeshes[0]->getMaterial()->getTextures().size() > 0){

    }

    return Mesh2D::load();
}
