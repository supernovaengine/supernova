
#ifndef TextureRender_h
#define TextureRender_h

#include "image/TextureData.h"
#include <vector>
#include <string>
#include <unordered_map>

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
        virtual void deleteTexture();
        
    };
    
}

#endif /* TextureRender_h */
