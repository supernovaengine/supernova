//
// (c) 2022 Eduardo Doria.
//

#ifndef POLYGON_COMPONENT_H
#define POLYGON_COMPONENT_H

namespace Supernova{

    struct PolygonPoint{
        Vector3 position;
        Vector4 color;
    };

    struct PolygonComponent{
        std::vector<PolygonPoint> points;

        bool needUpdatePolygon = true;
    };

}

#endif //POLYGON_COMPONENT_H