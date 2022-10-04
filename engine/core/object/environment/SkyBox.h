//
// (c) 2021 Eduardo Doria.
//

#ifndef SKYBOX_H
#define SKYBOX_H

#include "buffer/InterleavedBuffer.h"

#include "Scene.h"
#include "Entity.h"

namespace Supernova{
    class SkyBox{

    protected:
        Entity entity;
        Scene* scene;

        InterleavedBuffer buffer;

    public:
        SkyBox(Scene* scene);

        void setTextures(std::string textureFront, std::string textureBack,  
                        std::string textureLeft, std::string textureRight, 
                        std::string textureUp, std::string textureDown);

        void setTextureFront(std::string textureFront);
        void setTextureBack(std::string textureBack);
        void setTextureLeft(std::string textureLeft);
        void setTextureRight(std::string textureRight);
        void setTextureUp(std::string textureUp);
        void setTextureDown(std::string textureDown);

        void setColor(Vector4 color);
        void setColor(const float r, const float g, const float b);
        void setAlpha(const float alpha);
        Vector4 getColor();
        float getAlpha();

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

#endif //SKYBOX_H