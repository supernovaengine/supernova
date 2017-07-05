
#ifndef TextureManager_h
#define TextureManager_h

#include "render/TextureRender.h"
#include "image/TextureData.h"
#include "Texture.h"
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
        
        static Texture loadTexture(std::string relative_path);
        static Texture loadTexture(TextureData* textureData, std::string id);
        static Texture loadTextureCube(std::vector<std::string> relative_paths, std::string id);
        static void deleteUnused();

        static void clear();
    };
    
}

#endif /* TextureManager_h */
