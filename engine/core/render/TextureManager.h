
#ifndef TextureManager_h
#define TextureManager_h

#include "gles2/GLES2Texture.h"
#include "render/TextureRender.h"
#include "image/TextureFile.h"
#include <string>
#include <unordered_map>

class TextureManager {
    
    typedef struct {
        std::shared_ptr<TextureRender> value;
        bool hasAlphaChannel;
    } TextureStore;
    
private:

    typedef std::unordered_map<std::string, TextureStore>::iterator it_type;

    static std::unordered_map<std::string, TextureStore> textures;
    
    static TextureRender* getTextureRender();
    
    static TextureManager::it_type findToRemove();
public:
    
    static std::shared_ptr<TextureRender> loadTexture(std::string relative_path);
    static std::shared_ptr<TextureRender> loadTexture(TextureFile* textureFile, std::string id);
    static void deleteUnused();

    static bool hasAlphaChannel(std::string id);

    static void clear();
};

#endif /* TextureManager_h */
