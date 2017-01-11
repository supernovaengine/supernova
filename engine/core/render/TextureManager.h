
#ifndef TextureManager_h
#define TextureManager_h

#include "gles2/GLES2Texture.h"
#include "render/TextureRender.h"
#include "image/TextureFile.h"
#include <string>
#include <vector>

class TextureManager {
    
    typedef struct {
        std::shared_ptr<TextureRender> value;
        std::string key;
    } TextureStore;
    
private:
    
    static std::vector<TextureStore> textures;
    
    static TextureRender* getTextureRender();
    
    static int findToRemove();
public:
    
    static std::shared_ptr<TextureRender> loadTexture(std::string relative_path);
    static std::shared_ptr<TextureRender> loadTexture(TextureFile* textureFile, std::string id);
    static void deleteUnused();

    static void clear();
};

#endif /* TextureManager_h */
