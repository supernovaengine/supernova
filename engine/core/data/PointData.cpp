#include "PointData.h"


using namespace Supernova;


PointData::PointData(){
    
}


PointData::~PointData(){
    
}

void PointData::clearPoints(){
    
    positions.clear();
    normals.clear();
    textureRects.clear();
    sizes.clear();
    colors.clear();
    
}
