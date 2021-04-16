#ifndef OBJECT_H
#define OBJECT_H

#include "Scene.h"
#include "Entity.h"

namespace Supernova{

    class Object{
    protected:
        Entity entity;
        Scene* scene;

    public:
        Object(Scene* scene);
        virtual ~Object();

        Object* createChild();
        void addChild(Object* child);

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

        void updateTransform();
    };

}

#endif //OBJECT_H