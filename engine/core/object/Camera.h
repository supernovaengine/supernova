//
// (c) 2021 Eduardo Doria.
//

#ifndef CAMERA_H
#define CAMERA_H

#include "Object.h"
#include "component/CameraComponent.h"

namespace Supernova{

    class Camera: public Object{

    public:
        Camera(Scene* scene);
        Camera(Scene* scene, Entity entity);

        void activate();

        void setOrtho(float left, float right, float bottom, float top, float near, float far);
        void setPerspective(float y_fov, float aspect, float near, float far);

        void setType(CameraType type);
        CameraType getType() const;

        void setView(Vector3 view);
        void setView(const float x, const float y, const float z);
        Vector3 getView() const;

        void setUp(Vector3 up);
        void setUp(const float x, const float y, const float z);
        Vector3 getUp() const;

        void rotateView(float angle);
        void rotatePosition(float angle);

        void elevateView(float angle);
        void elevatePosition(float angle);

        void moveForward(float distance);
        void walkForward(float distance);
        void slide(float distance);

		void setRenderToTexture(bool renderToTexture);
		bool isRenderToTexture() const;

		Framebuffer* getFramebuffer();
		void setFramebufferSize(int width, int height);
		void setFramebufferFilter(TextureFilter filter);

        void updateCamera();
    };

}

#endif //CAMERA_H