//
// (c) 2022 Eduardo Doria.
//

#ifndef MESHPOLYGON_COMPONENT_H
#define MESHPOLYGON_COMPONENT_H

#include "math/Vector3.h"
#include "math/Vector4.h"
#include "component/PolygonComponent.h"

namespace Supernova{

    struct MeshPolygonComponent{
        std::vector<PolygonPoint> points;

        int width = 0;
        int height = 0;

        bool flipY = false;

        bool needUpdatePolygon = true;
    };

}

#endif //MESHPOLYGON_COMPONENT_H