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
    };
}

#endif //LINES_H