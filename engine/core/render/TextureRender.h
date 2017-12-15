
#ifndef TextureRender_h
#define TextureRender_h

#include "image/TextureData.h"
#include <vector>
#include <string>
#include <unordered_map>

#define TEXTURE_CUBE_FACE_POSITIVE_X 0
#define TEXTURE_CUBE_FACE_NEGATIVE_X 1
#define TEXTURE_CUBE_FACE_POSITIVE_Y 2
#define TEXTURE_CUBE_FACE_NEGATIVE_Y 3
#define TEXTURE_CUBE_FACE_POSITIVE_Z 4
#define TEXTURE_CUBE_FACE_NEGATIVE_Z 5

namespace Supernova {

    class TextureRender {
        
    private:
        
        typedef std::unordered_map< std::string, std::shared_ptr<TextureRender> >::iterator it_type;
        static std::unordered_map< std::string, std::shared_ptr<TextureRender> > texturesRender;
        
        bool loaded;
        
        int colorFormat;
        int width;
        int height;
        
        static TextureRender::it_type findToRemove();
        
    protected:
        
        TextureRender();
        
    public:
        
        virtual ~TextureRender();
        
        static std::shared_ptr<TextureRender> sharedInstance(std::string id);
        static void deleteUnused();
        
        bool isLoaded();
        
        int getColorFormat();
        int getWidth();
        int getHeight();
        
        virtual void loadTexture(TextureData* texturedata);
        virtual void loadTextureCube(std::vector<TextureData*> texturesdata);
        virtual void loadTextureFrame(int width, int height, bool depthframe);
        virtual void loadTextureFrameCube(int width, int height);
        
        virtual void initTextureFrame() = 0;
        virtual void initTextureFrame(int cubeFace) = 0;
        virtual void endTextureFrame() = 0;
        
        virtual void deleteTexture();
        
    };
    
}

#endif /* TextureRender_h */
