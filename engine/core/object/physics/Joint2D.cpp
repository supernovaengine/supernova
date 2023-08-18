//
// (c) 2023 Eduardo Doria.
//

#include "Joint2D.h"

#include "subsystem/PhysicsSystem.h"
#include "component/Body2DComponent.h"

using namespace Supernova;

Joint2D::Joint2D(Scene* scene, Entity entity): EntityHandle(scene, entity){

}

Joint2D::~Joint2D(){

}

Joint2D::Joint2D(const Joint2D& rhs): EntityHandle(rhs){
    
}

Joint2D& Joint2D::operator=(const Joint2D& rhs){

    return *this;
}