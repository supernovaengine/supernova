//
// (c) 2023 Eduardo Doria.
//

#ifndef OBJECT_H
#define OBJECT_H

#include "EntityHandle.h"

namespace Supernova{

    class Object: public EntityHandle{
    public:
        Object(Scene* scene);
        Object(Scene* scene, Entity entity);
        virtual ~Object();

        Object* createChild(); //TODO: add a template here
        void addChild(Object* child);
        void addChild(Entity child);

        void moveToFirst();
        void moveUp();
        void moveDown();
        void moveToLast();

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

        void setBillboard(bool billboard, bool fake = true, bool cylindrical = true);

        void setModelMatrix(Matrix4 modelMatrix);

        void updateTransform();
    };

}

#endif //OBJECT_H