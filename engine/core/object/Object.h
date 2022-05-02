//
// (c) 2021 Eduardo Doria.
//

#ifndef OBJECT_H
#define OBJECT_H

#include "Scene.h"
#include "Entity.h"

namespace Supernova{

    class Object{
    protected:
        Entity entity;
        Scene* scene;

        bool entityOwned;

    public:
        Object(Scene* scene);
        Object(Scene* scene, Entity entity);
        virtual ~Object();

        Object(const Object& rhs);
        Object& operator=(const Object& rhs);

        Object* createChild();
        void addChild(Object* child);

        void moveToFirst();
        void moveUp();
        void moveDown();
        void moveToLast();

        void setName(std::string name);
        std::string getName();

        void setPosition(Vector3 position);
        void setPosition(const float x, const float y, const float z);
        Vector3 getPosition();
        Vector3 getWorldPosition();

        void setRotation(Quaternion rotation);
        void setRotation(const float xAngle, const float yAngle, const float zAngle);
        Quaternion getRotation();
        Quaternion getWorldRotation();

        void setScale(const float factor);
        void setScale(Vector3 scale);
        Vector3 getScale();
        Vector3 getWorldScale();

        void setModelMatrix(Matrix4 modelMatrix);
    
        template <typename T>
        void addComponent(T &&component) {
            scene->addComponent<T>(entity, std::forward<T>(component));
        }
    
        template <typename T>
        void removeComponent() {
            scene->removeComponent<T>(entity);
        }
    
        template<typename T>
    	T& getComponent() {
    		return scene->getComponent<T>(entity);
    	}

        Entity getEntity();

        void updateTransform();
    };

}

#endif //OBJECT_H