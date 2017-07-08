#ifndef Texture_h
#define Texture_h

#define S_TEXTURE_2D 1
#define S_TEXTURE_CUBE 2

#include "render/TextureRender.h"
#include "image/ColorType.h"
#include <string>


namespace Supernova{

    class Texture{
    private:
        std::shared_ptr<TextureRender> textureRender;
        
        int type;
        
        std::string id;
        std::vector<TextureData*> texturesData;
        
    public:
        Texture();
        Texture(std::string path_id);
        Texture(TextureData* textureData, std::string id);
        Texture(std::vector<TextureData*> texturesData, std::string id);
        Texture(const Texture& t);
        
        virtual ~Texture();
        
        Texture& operator = (const Texture& t);
        
        void setId(std::string path_id);
        void setTextureData(TextureData* textureData);
        void setType(int type);

        std::string getId();
        int getType();
        std::shared_ptr<TextureRender> getTextureRender();
        
        int getColorFormat();
        bool hasAlphaChannel();
        int getWidth();
        int getHeight();
        
        bool load();
        void destroy();
    };
    
}

#endif /* Texture_h */
