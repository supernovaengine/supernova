
#ifndef TextureManager_h
#define TextureManager_h

#include "render/TextureRender.h"
#include "image/TextureData.h"
#include "image/Texture.h"
#include <string>
#include <unordered_map>

namespace Supernova {

    class TextureManager {
    private:

        typedef std::unordered_map<std::string, Texture>::iterator it_type;

        static std::unordered_map<std::string, Texture> textures;
        
        static TextureRender* getTextureRender();
        
        static TextureManager::it_type findToRemove();
    public:
        
        static std::shared_ptr<TextureRender> loadTexture(std::string relative_path);
        static std::shared_ptr<TextureRender> loadTexture(TextureData* textureData, std::string id);
        static std::shared_ptr<TextureRender> loadTextureCube(std::vector<std::string> relative_paths, std::string id);
        static void deleteUnused();

        static bool hasAlphaChannel(std::string id);
        static int getTextureWidth(std::string id);
        static int getTextureHeight(std::string id);

        static void clear();
    };
    
}

#endif /* TextureManager_h */
