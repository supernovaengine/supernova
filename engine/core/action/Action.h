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

    public:
        Action(Scene* scene);

        void start();
        void pause();
        void stop();

        void setTarget(Entity target);

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
    };
}

#endif //ACTION_H