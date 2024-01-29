//
// (c) 2024 Eduardo Doria.
//

#ifndef LINES_H
#define LINES_H

#include "Object.h"

namespace Supernova{

    class Lines: public Object{
    public:
        Lines(Scene* scene);
        virtual ~Lines();

        bool load();

        void addLine(Vector3 pointA, Vector3 pointB);
        void addLine(Vector3 pointA, Vector3 pointB, Vector3 color);
        void addLine(Vector3 pointA, Vector3 pointB, Vector4 color);
        void addLine(Vector3 pointA, Vector3 pointB, Vector4 colorA, Vector4 colorB);

        LineData getLine(size_t index) const;
        void setLine(size_t index, LineData lineData);

        void setLinePointA(size_t index, Vector3 pointA);
        void setLinePointB(size_t index, Vector3 pointB);
        void setLineColorA(size_t index, Vector4 colorA);
        void setLineColorB(size_t index, Vector4 colorB);

        void clearLines();
    };
}

#endif //LINES_H