//
// (c) 2023 Eduardo Doria.
//

#ifndef ENTITYHANDLE_H
#define ENTITYHANDLE_H

#include "Scene.h"
#include "Entity.h"

namespace Supernova{
    class EntityHandle{

    protected:
        Entity entity;
        Scene* scene;

        bool entityOwned;

    public:
        EntityHandle(Scene* scene);
        EntityHandle(Scene* scene, Entity entity);
        virtual ~EntityHandle();

        EntityHandle(const EntityHandle& rhs);
        EntityHandle& operator=(const EntityHandle& rhs);

        Scene* getScene() const;
        Entity getEntity() const;

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

    };
}

#endif //ENTITYHANDLE_H