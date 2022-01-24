//
// (c) 2022 Eduardo Doria.
//

#ifndef POLYGON_COMPONENT_H
#define POLYGON_COMPONENT_H

#include "math/Vector3.h"
#include "math/Vector4.h"

namespace Supernova{

    struct Point{
        Vector3 position;
        Vector4 color;
    };

    struct PolygonComponent{
        std::vector<Point> points;

        int width = 0;
        int height = 0;

        bool flipY = false;

        bool needUpdatePolygon = true;
    };

}

#endif //POLYGON_COMPONENT_H