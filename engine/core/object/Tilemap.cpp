//
// (c) 2023 Eduardo Doria.
//

#include "Tilemap.h"

using namespace Supernova;

Tilemap::Tilemap(Scene* scene): Mesh(scene){
    addComponent<TilemapComponent>({});
}

Tilemap::~Tilemap(){

}