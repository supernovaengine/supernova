#ifndef object_h
#define object_h

#include <vector>
#include "Render.h"
#include "math/Matrix4.h"
#include "math/Vector3.h"
#include "math/Quaternion.h"

namespace Supernova {

    class Scene;

    class Object: public Render {

    private:
        
        bool firstLoaded;
        
        void setSceneAndConfigure(Scene* scene);
        void removeScene();
        int findObject(Object* object);
        void setDepth(bool depth);
        
    protected:
        
        bool loaded;

        std::vector<Object*> objects;

        Object* parent;
        Scene* scene;

        Matrix4* viewMatrix;
        Matrix4* projectionMatrix;
        Matrix4* viewProjectionMatrix;
        Vector3* cameraPosition;

        Matrix4 modelMatrix;
        Matrix4 modelViewProjectionMatrix;

        Vector3 position;
        Quaternion rotation;
        Vector3 scale;
        Vector3 center;

        Vector3 worldPosition;
        Quaternion worldRotation;
        
        bool reload();

        virtual void updateMVPMatrix();

    public:
        Object();
        virtual ~Object();

        void addObject(Object* obj);
        void removeObject(Object* obj);

        Matrix4 getModelMatrix();
        Matrix4 getModelViewProjectMatrix();
        Vector3 getCameraPosition();

        void setPosition(const float x, const float y, const float z);
        void setPosition(Vector3 position);
        Vector3 getPosition();
        Vector3 getWorldPosition();

        void setRotation(const float xAngle, const float yAngle, const float zAngle);
        void setRotation(Quaternion rotation);
        Quaternion getRotation();
        Quaternion getWorldRotation();

        void setScale(const float factor);
        void setScale(Vector3 scale);
        Vector3 getScale();

        void setCenter(const float x, const float y, const float z);
        void setCenter(Vector3 center);
        Vector3 getCenter();
        
        Scene* getScene();
        Object* getParent();
        
        bool isLoaded();
        
        void moveToFront();
        void moveToBack();
        void moveUp();
        void moveDown();
        
        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
        virtual void updateMatrix();

        virtual bool load();
        virtual bool draw();
        virtual void destroy();

    };
    
}

#endif /* object_h */
