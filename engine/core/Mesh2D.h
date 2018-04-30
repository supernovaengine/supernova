

#ifndef _Mesh2D_h
#define _Mesh2D_h

#include "Mesh.h"

namespace Supernova {

    class Mesh2D: public Mesh {

    protected:
        int width;
        int height;

        bool billboard;
        bool fixedSizeBillboard;
        float billboardScaleFactor;

        bool clipping;
        bool invertTexture;

    public:
        Mesh2D();
        virtual ~Mesh2D();

        void setBillboard(bool billboard);
        void setFixedSizeBillboard(bool fixedSizeBillboard);
        void setBillboardScaleFactor(float billboardScaleFactor);

        void setClipping(bool clipping);

        void setWidth(int width);
        int getWidth();

        void setHeight(int height);
        int getHeight();
        
        virtual void setSize(int width, int height);
        virtual void setInvertTexture(bool invertTexture);
        
        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
        virtual void updateMatrix();

        virtual bool draw();
    };
    
}

#endif /* _Mesh2D_hpp */
