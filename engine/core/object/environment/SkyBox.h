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

        void setTextures(const std::string& id,
                        TextureData textureFront, TextureData textureBack,
                        TextureData textureLeft, TextureData textureRight,
                        TextureData textureUp, TextureData textureDown);

        void setTextures(const std::string& textureFront, const std::string& textureBack,
                        const std::string& textureLeft, const std::string& textureRight,
                        const std::string& textureUp, const std::string& textureDown);

        void setTextureFront(const std::string& textureFront);
        void setTextureBack(const std::string& textureBack);
        void setTextureLeft(const std::string& textureLeft);
        void setTextureRight(const std::string& textureRight);
        void setTextureUp(const std::string& textureUp);
        void setTextureDown(const std::string& textureDown);

        void setColor(Vector4 color);
        void setColor(const float r, const float g, const float b);
        void setColor(const float r, const float g, const float b, const float a);
        void setAlpha(const float alpha);
        Vector4 getColor() const;
        float getAlpha() const;

        void setRotation(float angle);
        float getRotation() const;
    };
}

#endif //SKYBOX_H