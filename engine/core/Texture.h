#ifndef Texture_h
#define Texture_h

#define S_TEXTURE_2D 1
#define S_TEXTURE_CUBE 2
#define S_TEXTURE_FRAME 3
#define S_TEXTURE_DEPTH_FRAME 4

#include "render/TextureRender.h"
#include "image/ColorType.h"
#include <string>


namespace Supernova{

    class Texture{
    private:
        std::shared_ptr<TextureRender> textureRender;
        
        int type;

        int textureFrameWidth;
        int textureFrameHeight;
        
        std::string id;
        std::vector<TextureData*> texturesData;
        
        bool dataOwned;
        
    public:
        Texture();
        Texture(std::string path_id);
        Texture(TextureData* textureData, std::string id);
        Texture(std::vector<TextureData*> texturesData, std::string id);
        Texture(int textureFrameWidth, int textureFrameHeight, std::string id);
        Texture(const Texture& t);
        
        virtual ~Texture();
        
        Texture& operator = (const Texture& t);
        
        void setId(std::string id);
        void setTextureData(TextureData* textureData);
        void setType(int type);
        void setDataOwned(bool dataOwned);
        void setTextureFrameSize(int textureFrameWidth, int textureFrameHeight);

        std::string getId();
        int getType();
        std::shared_ptr<TextureRender> getTextureRender();
        bool getDataOwned();
        int getTextureFrameWidth();
        int getTextureFrameHeight();
        
        int getColorFormat();
        bool hasAlphaChannel();
        int getWidth();
        int getHeight();
        
        bool load();
        void destroy();
    };
    
}

#endif /* Texture_h */
