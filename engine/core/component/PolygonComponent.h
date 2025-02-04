//
// (c) 2024 Eduardo Doria.
//

#ifndef POLYGON_COMPONENT_H
#define POLYGON_COMPONENT_H

namespace Supernova{

    struct PolygonPoint{
        Vector3 position;
        Vector4 color;
    };

    struct SUPERNOVA_API PolygonComponent{
        std::vector<PolygonPoint> points;

        bool needUpdatePolygon = true;
    };

}

#endif //POLYGON_COMPONENT_H