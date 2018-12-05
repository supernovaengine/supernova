//
// (c) 2018 Eduardo Doria.
//

#ifndef LINES_H
#define LINES_H

#include "Mesh2D.h"

namespace Supernova {

    class Lines : public GraphicObject {

    private:
        float lineWidth;

    public:
        Lines();
        virtual ~Lines();

        void addLine(Vector3 pointA, Vector3 pointB);
        void clearLines();

        float getLineWidth() const;
        void setLineWidth(float lineWidth);

        virtual bool renderDraw();
        virtual bool load();

    };

}


#endif //LINES_H
