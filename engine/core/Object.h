#ifndef object_h
#define object_h

//
// (c) 2018 Eduardo Doria.
//

#include <vector>
#include "Render.h"
#include "math/Matrix4.h"
#include "math/Vector3.h"
#include "math/Vector2.h"
#include "math/Quaternion.h"
#include "action/Action.h"
#include "physics/Body2D.h"

namespace Supernova {

    class Scene;

    class Object: public Render {

    private:
        
        bool firstLoaded;
        bool markToUpdate;

        bool ownedBody;
        bool allowBodyUpdate;
        
        void setSceneAndConfigure(Scene* scene);
        void removeScene();
        void setSceneDepth(bool depth);
        
    protected:
        
        bool loaded;

        std::vector<Object*> objects;

        std::vector<Action*> actions;

        Object* parent;
        Scene* scene;

        Matrix4* viewMatrix;
        Matrix4* projectionMatrix;
        Matrix4* viewProjectionMatrix;
        Vector3 cameraPosition;

        Matrix4 transformMatrix;
        Matrix4 modelMatrix;
        Matrix4 modelViewProjectionMatrix;

        Vector3 position;
        Quaternion rotation;
        Vector3 scale;
        Vector3 center;

        Vector3 worldPosition;
        Quaternion worldRotation;
        Vector3 worldScale;

        Body* body;
        
        bool reload();

        virtual void updateMVPMatrix();
        virtual void updateModelMatrix();
        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);

    public:
        Object();
        virtual ~Object();

        Object* getObject(unsigned int index) const;
        const std::vector<Object *> &getObjects() const;

        void addObject(Object* obj);
        void removeObject(Object* obj);

        void addAction(Action* action);
        void removeAction(Action* action);

        Matrix4 getTransformMatrix();
        Matrix4 getModelMatrix();
        Matrix4 getModelViewProjectMatrix();
        Vector3 getCameraPosition();

        virtual void setPosition(Vector3 position);
        void setPosition(const float x, const float y, const float z);
        void setPosition(Vector2 position);
        void setPosition(const float x, const float y);
        Vector3 getPosition();
        Vector3 getWorldPosition();

        virtual void setRotation(Quaternion rotation);
        void setRotation(const float xAngle, const float yAngle, const float zAngle);
        Quaternion getRotation();
        Quaternion getWorldRotation();

        void setScale(const float factor);
        void setScale(Vector3 scale);
        Vector3 getScale();
        Vector3 getWorldScale();

        void setCenter(const float x, const float y, const float z);
        void setCenter(Vector3 center);
        void setCenter(const float x, const float y);
        void setCenter(Vector2 center);
        Vector3 getCenter();
        
        Scene* getScene();
        Object* getParent();
        
        bool isIn3DScene();
        bool isLoaded();

        int find(Object* object);
        void moveTo(int index);
        void moveToFirst();
        void moveToLast();
        void moveUp();
        void moveDown();

        void setOwnedBody(bool ownedBody);

        Body2D* createBody2D();
        //Body3D* createBody3D();

        void setBody(Body* body);
        Body* getBody();

        void updateBodyFromObject();
        void updateFromBody();

        virtual void needUpdate();

        virtual bool load();
        virtual bool draw();
        virtual void update();
        virtual void destroy();

    };
    
}

#endif /* object_h */
