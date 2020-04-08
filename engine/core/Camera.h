#ifndef camera_h
#define camera_h

#define S_CAMERA_2D 0
#define S_CAMERA_ORTHO 1
#define S_CAMERA_PERSPECTIVE 2

#include "math/Matrix4.h"
#include "math/Vector4.h"
#include "math/Vector3.h"
#include "math/Vector2.h"
#include "math/Ray.h"
#include "math/Rect.h"
#include "math/AlignedBox.h"
#include "Object.h"

namespace Supernova {

    class Scene;

    class Camera: public Object {

        enum FrustumPlane
        {
            FRUSTUM_PLANE_NEAR   = 0,
            FRUSTUM_PLANE_FAR    = 1,
            FRUSTUM_PLANE_LEFT   = 2,
            FRUSTUM_PLANE_RIGHT  = 3,
            FRUSTUM_PLANE_TOP    = 4,
            FRUSTUM_PLANE_BOTTOM = 5
        };

    private:
        Matrix4 projectionMatrix;
        Matrix4 viewMatrix;
        Matrix4 viewProjectionMatrix;

        bool needUpdateFrustumPlanes;

    protected:

        Vector3 view;
        Vector3 up;

        Vector3 worldView;
        Vector3 worldUp;
        Vector3 worldRight;

        float left;
        float right;
        float bottom;
        float top;
        float orthoNear;
        float orthoFar;

        float y_fov;
        float aspect;
        float perspectiveNear;
        float perspectiveFar;

        int type;

        bool automatic;

        Scene* linkedScene;

        Plane frustumPlanes[6];

    public:

        Camera();
        Camera(const Camera &camera);
        Camera(int type);
        virtual ~Camera();

        Camera& operator=(const Camera &c);

        void setView(const float x, const float y, const float z);
        void setView(Vector3 view);
        Vector3 getView();

        void setUp(const float x, const float y, const float z);
        void setUp(Vector3 up);
        Vector3 getUp();

        void setType(int type);
        int getType();

        float getFOV();
        float getAspect();
        float getTop();
        float getBottom();
        float getLeft();
        float getRight();

        void setNear(float near);
        float getNear();

        void setFar(float far);
        float getFar();
        Vector2 getNearFarPlane();
        
        void updateAutomaticSizes(Rect rect);
        
        void setOrtho(float left, float right, float bottom, float top, float near, float far);
        void setPerspective(float y_fov, float aspect, float near, float far);

        void rotateView(float angle);
        void rotatePosition(float angle);

        void elevateView(float angle);
        void elevatePosition(float angle);

        void moveForward(float distance);
        void walkForward(float distance);
        void slide(float distance);

        void setLinkedScene(Scene* linkedScene);

        Matrix4* getViewMatrix();
        Matrix4* getProjectionMatrix();
        Matrix4* getViewProjectionMatrix();
        Vector3* getWorldPositionPtr();
        Vector3* getWorldUpPtr();
        Vector3* getWorldViewPtr();
        Vector3* getWorldRightPtr();

        Ray pointsToRay(float normalized_x, float normalized_y);

        bool isInside(const AlignedBox& box);
        bool isInside(const Vector3& point);
        bool isInside(const Vector3& center, const float& radius);

        bool updateFrustumPlanes();
        void updateViewMatrix();
        void updateProjectionMatrix();
        void updateViewProjectionMatrix();
        virtual void updateModelMatrix();

    };
    
}
#endif /* camera_h */
