//
// (c) 2023 Eduardo Doria.
//

#include "Contact2D.h"

#include "box2d.h"

using namespace Supernova;

Contact2D::Contact2D(b2Contact* contact){
    this->contact = contact;
}

Contact2D::~Contact2D(){

}

Contact2D::Contact2D(const Contact2D& rhs){
    
}

Contact2D& Contact2D::operator=(const Contact2D& rhs){

    return *this;
}