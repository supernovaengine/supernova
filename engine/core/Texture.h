#ifndef Texture_h
#define Texture_h

#define S_TEXTURE_2D 0
#define S_TEXTURE_CUBE 1
#define S_TEXTURE_FRAME 2
#define S_TEXTURE_DEPTH_FRAME 3
#define S_TEXTURE_FRAME_CUBE 4

#define S_COLOR_GRAY  1
#define S_COLOR_RGB_ALPHA  2
#define S_COLOR_GRAY_ALPHA  3
#define S_COLOR_RGB  4
#define S_COLOR_ALPHA 5

#include "render/TextureRender.h"
#include <string>


namespace Supernova{

    class Texture{
    private:
        std::shared_ptr<TextureRender> textureRender;
        
        int type;

        int textureFrameWidth;
        int textureFrameHeight;
        
        std::string id;
        std::vector<std::string> paths;
        std::vector<TextureData*> texturesData;
        
        bool dataOwned;
        bool preserveData;

        bool resampleToPowerOfTwo;
        bool nearestScale;

        bool userResampleToPowerOfTwo;
        bool userNearestScale;

        void setDefaults();
        void releaseData();
        
    public:
        Texture();
        Texture(std::string filename);
        Texture(std::vector<std::string> paths, std::string id = "");
        Texture(TextureData* textureData, std::string id = "");
        Texture(std::vector<TextureData*> texturesData, std::string id = "");
        Texture(int textureFrameWidth, int textureFrameHeight, std::string id = "");
        Texture(const Texture& t);
        
        virtual ~Texture();
        
        Texture& operator = (const Texture& t);

        static bool isIdLoaded(std::string id);
        
        void setId(std::string id);
        void setTextureData(TextureData* textureData);
        void setType(int type);
        void setDataOwned(bool dataOwned);
        void setTextureFrameSize(int textureFrameWidth, int textureFrameHeight);
        void setResampleToPowerOfTwo(bool resampleToPowerOfTwo);
        void setNearestScale(bool nearestScale);

        void setPreserveData(bool preserveData);
        bool isPreserveData() const;

        std::string getId();
        int getType();
        std::shared_ptr<TextureRender> getTextureRender();
        bool getDataOwned();
        int getTextureFrameWidth();
        int getTextureFrameHeight();
        bool getResampleToPowerOfTwo();
        bool getNearestScale();
        TextureData* getTextureData(int index = 0);
        
        int getColorFormat();
        bool hasAlphaChannel();
        int getWidth();
        int getHeight();
        
        bool load();
        void destroy();
    };
    
}

#endif /* Texture_h */
