//
// (c) 2021 Eduardo Doria.
//

#ifndef ACTION_H
#define ACTION_H

#include "Scene.h"
#include "Entity.h"

namespace Supernova{
    class Action{
    protected:
        Entity entity;
        Scene* scene;

        bool entityOwned;

    public:
        Action(Scene* scene);
        Action(Scene* scene, Entity entity);
        virtual ~Action();

        Action(const Action& rhs);
        Action& operator=(const Action& rhs);

        void start();
        void pause();
        void stop();

        void setTarget(Entity target);
        Entity getTarget() const;

        void setSpeed(float speed);
        float getSpeed() const;

        bool isRunning() const;

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
    };
}

#endif //ACTION_H