#ifndef camera_h
#define camera_h

#define S_CAMERA_2D 0
#define S_CAMERA_ORTHO 1
#define S_CAMERA_PERSPECTIVE 2

#include "math/Matrix4.h"
#include "math/Vector4.h"
#include "math/Vector3.h"
#include "math/Ray.h"
#include "math/Rect.h"
#include "Object.h"

namespace Supernova {

    class Camera: public Object {

    private:
        Matrix4 projectionMatrix;
        Matrix4 viewMatrix;
        Matrix4 viewProjectionMatrix;

    protected:

        Vector3 view;
        Vector3 up;

        Vector3 worldView;
        Vector3 worldUp;

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

        Object* sceneObject;

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

        void setSceneObject(Object* scene);

        virtual void updateMatrix();

        Matrix4* getViewMatrix();
        Matrix4* getProjectionMatrix();
        Matrix4* getViewProjectionMatrix();

        Ray pointsToRay(float normalized_x, float normalized_y);

    };
    
}
#endif /* camera_h */
