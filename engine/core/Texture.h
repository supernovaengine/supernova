#ifndef Texture_h
#define Texture_h

#include "render/TextureRender.h"
#include "image/ColorType.h"

namespace Supernova{

    class Texture{
    private:
        std::shared_ptr<TextureRender> textureRender;
        int colorFormat;
        int width;
        int height;
        
    public:
        Texture();
        Texture(std::shared_ptr<TextureRender> textureRender, int colorFormat, int width, int height);
        Texture(const Texture& t);
        virtual ~Texture();

        Texture& operator = (const Texture& t);
        
        void setTextureRender(std::shared_ptr<TextureRender> textureRender);
        void setColorFormat(int colorFormat);
        void setSize(int width, int height);

        std::shared_ptr<TextureRender> getTextureRender();
        int getColorFormat();
        bool hasAlphaChannel();
        int getWidth();
        int getHeight();

        void destroy();
    };
    
}

#endif /* Texture_h */
