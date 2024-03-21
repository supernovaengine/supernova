//
// (c) 2021 Eduardo Doria.
//

#ifndef CAMERA_H
#define CAMERA_H

#include "Object.h"
#include "component/CameraComponent.h"
#include "math/Ray.h"

namespace Supernova{

    class Camera: public Object{

    public:
        Camera(Scene* scene);
        Camera(Scene* scene, Entity entity);

        void activate();

        void setOrtho(float left, float right, float bottom, float top, float near, float far);
        void setPerspective(float yfov, float aspect, float near, float far);

        void setNear(float near);
        float getNear() const;

        void setFar(float far);
        float getFar() const;

        void setLeft(float left);
        float getLeft() const;

        void setRight(float right);
        float getRight() const;

        void setBottom(float bottom);
        float getBottom() const;

        void setTop(float top);
        float getTop() const;

        void setAspect(float aspect);
        float getAspect() const;

        void setYFov(float yfov);
        float getYFov() const;

        void setType(CameraType type);
        CameraType getType() const;

        void setView(Vector3 view);
        void setView(const float x, const float y, const float z);
        Vector3 getView() const;

        void setUp(Vector3 up);
        void setUp(const float x, const float y, const float z);
        Vector3 getUp() const;

        Vector3 getWorldView() const;
        Vector3 getWorldUp() const;
        Vector3 getWorldRight() const;

        void rotateView(float angle);
        void rotatePosition(float angle);

        void elevateView(float angle);
        void elevatePosition(float angle);

        void moveForward(float distance);
        void walkForward(float distance);
        void slide(float distance);
        void zoom(float distance);

        void setRenderToTexture(bool renderToTexture);
        bool isRenderToTexture() const;

        Framebuffer* getFramebuffer();
        void setFramebufferSize(int width, int height);
        void setFramebufferFilter(TextureFilter filter);

        void setTransparentSort(bool transparentSort);
        bool isTransparentSort() const;

        Ray screenToRay(float x, float y);

        void updateCamera();
    };

}

#endif //CAMERA_H