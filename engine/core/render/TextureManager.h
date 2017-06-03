
#ifndef TextureManager_h
#define TextureManager_h

#include "render/TextureRender.h"
#include "image/TextureFile.h"
#include <string>
#include <unordered_map>

class TextureManager {
    
    typedef struct {
        std::shared_ptr<TextureRender> value;
        bool hasAlphaChannel;
        int width;
        int height;
    } TextureStore;
    
private:

    typedef std::unordered_map<std::string, TextureStore>::iterator it_type;

    static std::unordered_map<std::string, TextureStore> textures;
    
    static TextureRender* getTextureRender();
    
    static TextureManager::it_type findToRemove();
public:
    
    static std::shared_ptr<TextureRender> loadTexture(std::string relative_path);
    static std::shared_ptr<TextureRender> loadTexture(TextureFile* textureFile, std::string id);
    static std::shared_ptr<TextureRender> loadTextureCube(std::vector<std::string> relative_paths, std::string id);
    static void deleteUnused();

    static bool hasAlphaChannel(std::string id);
    static int getTextureWidth(std::string id);
    static int getTextureHeight(std::string id);

    static void clear();
};

#endif /* TextureManager_h */
