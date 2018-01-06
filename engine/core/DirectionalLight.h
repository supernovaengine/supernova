#ifndef directionallight_h
#define directionallight_h

#include "Light.h"
#include "math/Vector3.h"

namespace Supernova {

    class DirectionalLight: public Light {

    private:

        void configLightOrthoCamera(Camera* lightCamera, Camera* sceneCamera);

    protected:

        Vector2 shadowCameraNearFar;

        virtual void updateLightCamera();

    public:

        DirectionalLight();
        virtual ~DirectionalLight();

        void setDirection(Vector3 direction);
        void setDirection(float x, float y, float z);

        Vector2 getShadowCameraNearFar();

        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);

        virtual bool loadShadow();

    };
    
}

#endif /* directionallight_h */
