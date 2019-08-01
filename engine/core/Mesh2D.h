

#ifndef _Mesh2D_h
#define _Mesh2D_h

#include "Mesh.h"

namespace Supernova {

    class Mesh2D: public Mesh {

    private:
        Vector3 billboardOldScale;

    protected:
        int width;
        int height;

        bool billboard;
        bool fakeBillboard;
        bool fixedSizeBillboard;
        bool cylindricalBillboard;
        float billboardScaleFactor;

        bool clipping;
        float clipBorder[4]; //0-left, 1-top, 2-right, 3-bottom
        bool invertTexture;

        float convTex(float value);

        virtual void updateMVPMatrix();

    public:
        Mesh2D();
        virtual ~Mesh2D();

        void setBillboard(bool billboard);
        void setFakeBillboard(bool fakeBillboard);
        void setFixedSizeBillboard(bool fixedSizeBillboard);
        void setBillboardScaleFactor(float billboardScaleFactor);
        void setCylindricalBillboard(bool cylindricalBillboard);

        void setClipping(bool clipping);
        void setClipBorder(float left, float top, float right, float bottom);

        void setWidth(int width);
        int getWidth();

        void setHeight(int height);
        int getHeight();
        
        virtual void setSize(int width, int height);
        virtual void setInvertTexture(bool invertTexture);
        
        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
        virtual void updateModelMatrix();

        virtual bool draw();
    };
    
}

#endif /* _Mesh2D_hpp */
