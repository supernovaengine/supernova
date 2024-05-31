//
// (c) 2024 Eduardo Doria.
//

#ifndef SKYBOX_H
#define SKYBOX_H

#include "object/EntityHandle.h"

namespace Supernova{
    class SkyBox: public EntityHandle{

    public:
        SkyBox(Scene* scene);

        bool load();

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
        void setColor(const float r, const float g, const float b, const float a);
        void setAlpha(const float alpha);
        Vector4 getColor() const;
        float getAlpha() const;

    };
}

#endif //SKYBOX_H