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
        std::string getName() const;

        void setPosition(Vector3 position);
        void setPosition(const float x, const float y, const float z);
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
    	T& getComponent() const {
    		return scene->getComponent<T>(entity);
    	}

        Entity getEntity() const;

        void updateTransform();
    };

}

#endif //OBJECT_H