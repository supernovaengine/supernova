#ifndef Texture_h
#define Texture_h

#include "render/TextureRender.h"

namespace Supernova{

    class Texture{
    private:
        std::shared_ptr<TextureRender> textureRender;
        bool alphaChannel;
        int width;
        int height;
        
    public:
        Texture();
        Texture(std::shared_ptr<TextureRender> textureRender, bool alphaChannel, int width, int height);
        virtual ~Texture();
        
        void setTextureRender(std::shared_ptr<TextureRender> textureRender);
        void setAlphaChannel(bool alphaChannel);
        void setSize(int width, int height);
        
        std::shared_ptr<TextureRender> getTextureRender();
        bool hasAlphaChannel();
        int getWidth();
        int getHeight();
    };
    
}

#endif /* Texture_h */
