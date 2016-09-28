
#ifndef TextureManager_h
#define TextureManager_h

#include "gles2/GLES2Texture.h"
#include "render/TextureRender.h"

class TextureManager {
    
    typedef struct {
        TextureRender* value;
        std::string key;
        int reference;
    } TextureStore;
    
private:
    
    static std::vector<TextureStore> textures;
    
    TextureRender* getTextureRender();
public:
    
    TextureRender* loadTexture(std::string relative_path);
    void deleteTexture(std::string relative_path);

    static void clear();
};

#endif /* TextureManager_h */
