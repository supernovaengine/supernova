#ifndef light_h
#define light_h

#define S_POINT_LIGHT 1
#define S_DIRECTIONAL_LIGHT 2
#define S_SPOT_LIGHT 3

#include "Object.h"
#include "Texture.h"
#include "Camera.h"

namespace Supernova {

    class Light: public Object {

    protected:

        Vector3 color;
        Vector3 target;
        Vector3 direction;
        float power;
        float spotAngle;

        Vector3 worldTarget;

        int type;

        bool useShadow;
        Texture* shadowMap;
        std::vector<Camera*> lightCameras;
        float shadowBias;

        int shadowMapWidth;
        int shadowMapHeight;

        Matrix4 depthVPMatrix;

        virtual void updateLightCamera();

    public:

        Light();
        Light(int type);
        virtual ~Light();

        int getType();
        Vector3 getColor();
        Vector3 getTarget();
        Vector3 getDirection();
        float getPower();
        float getSpotAngle();
        bool isUseShadow();
        Camera* getLightCamera();
        Camera* getLightCamera(int index);
        Texture* getShadowMap();
        Matrix4 getDepthVPMatrix();
        float getShadowBias();

        Vector3 getWorldTarget();

        void setPower(float power);
        void setColor(Vector3 color);
        void setUseShadow(bool useShadow);
        void setShadowBias(float shadowBias);

        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
        virtual void updateMatrix();

        virtual bool loadShadow();

    };
    
}

#endif /* light_hpp */
