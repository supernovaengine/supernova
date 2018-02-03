#ifndef directionallight_h
#define directionallight_h

#include "Light.h"
#include "math/Vector3.h"

namespace Supernova {

    class DirectionalLight: public Light {

    private:

        Matrix4 sceneCameraViewProjection;

        void configLightOrthoCamera(Camera* lightCamera, Matrix4 sceneCameraInv);
        float lerp(float a, float b, float fraction);

    protected:

        int numShadowCascades;
        float shadowSplitLogFactor;
        std::vector<Vector2> cascadeCameraNearFar;

        virtual void updateLightCamera();

    public:

        DirectionalLight();
        virtual ~DirectionalLight();

        void setDirection(Vector3 direction);
        void setDirection(float x, float y, float z);

        Vector2 getCascadeCameraNearFar(int index);
        int getNumShadowCasdades();
        float getShadowSplitLogFactor();

        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);

        virtual bool loadShadow();

    };
    
}

#endif /* directionallight_h */
