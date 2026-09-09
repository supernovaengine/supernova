// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef POLYGON_COMPONENT_H
#define POLYGON_COMPONENT_H

#include <vector>

namespace doriax{

    struct PolygonPoint{
        Vector3 position;
        Vector4 color;
    };

    struct DORIAX_API PolygonComponent{
        std::vector<PolygonPoint> points;

        bool needUpdatePolygon = true;
    };

}

#endif //POLYGON_COMPONENT_H