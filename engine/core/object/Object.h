//
// (c) 2024 Eduardo Doria.
//

#ifndef OBJECT_H
#define OBJECT_H

#include "EntityHandle.h"
#include "Body2D.h"
#include "Body3D.h"
#include "math/Ray.h"

namespace Supernova{

    class Object: public EntityHandle{
    public:
        Object(Scene* scene);
        Object(Scene* scene, Entity entity);
        virtual ~Object();

        void addChild(Object* child);
        void addChild(Entity child);

        void moveToTop();
        void moveUp();
        void moveDown();
        void moveToBottom();

        void setName(std::string name);
        std::string getName() const;

        void setPosition(Vector3 position);
        void setPosition(const float x, const float y, const float z);
        void setPosition(const float x, const float y);
        Vector3 getPosition() const;
        Vector3 getWorldPosition() const;

        void setRotation(Quaternion rotation);
        void setRotation(const float xAngle, const float yAngle, const float zAngle);
        Quaternion getRotation() const;
        Quaternion getWorldRotation() const;

        void setScale(const float factor);
        void setScale(Vector3 scale);
        Vector3 getScale() const;
        Vector3 getWorldScale() const;

        void setVisible(bool visible);
        bool isVisible() const;
        void setVisibleOnly(bool visible);

        void setLocalMatrix(Matrix4 localMatrix);
        Matrix4 getLocalMatrix() const;

        Matrix4 getModelMatrix() const;
        Matrix4 getNormalMatrix() const;
        Matrix4 getModelViewProjectionMatrix() const;

        void updateTransform();

        // 2D physics
        Body2D getBody2D();
        void removeBody2D();

        // 3D physics
        Body3D getBody3D();
        void removeBody3D();

        Ray getRay(Vector3 direction);
    };

}

#endif //OBJECT_H