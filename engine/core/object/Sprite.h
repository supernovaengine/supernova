//
// (c) 2021 Eduardo Doria.
//

#ifndef SPRITE_H
#define SPRITE_H

#include "buffer/InterleavedBuffer.h"
#include "buffer/IndexBuffer.h"

#include "Object.h"
#include "Scene.h"
#include "Entity.h"

namespace Supernova{
    class Sprite: public Object{

    protected:
        //Entity entity;
        //Scene* scene;

        InterleavedBuffer buffer;
        IndexBuffer indices;

    public:
        Sprite(Scene* scene);

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

    };
}

#endif //SPRITE_H