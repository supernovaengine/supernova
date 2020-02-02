//
// (c) 2018 Eduardo Doria.
//

#ifndef LINES_H
#define LINES_H

#include "Mesh2D.h"

namespace Supernova {

    class Lines : public GraphicObject {

    private:
        InterleavedBuffer buffer;

        Vector4 color;

        float lineWidth;

    public:
        Lines();
        virtual ~Lines();

        virtual void setColor(Vector4 color);
        virtual void setColor(float red, float green, float blue, float alpha);
        virtual Vector4 getColor();

        void addLine(Vector3 pointA, Vector3 pointB);
        void clearLines();

        float getLineWidth() const;
        void setLineWidth(float lineWidth);

        virtual bool load();

    };

}


#endif //LINES_H
