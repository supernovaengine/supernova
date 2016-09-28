#ifndef gles2texture_h
#define gles2texture_h

#include "GLES2Header.h"
#include "image/TextureLoader.h"
#include "render/TextureRender.h"

class GLES2Texture : public TextureRender{

private:
    GLuint gTexture;
    
    GLenum getGlColorFormat(const int color_format);
    GLuint getTextureObjectId(const GLsizei width, const GLsizei height, const GLenum type, const GLvoid* pixels);

public:

    void loadTexture(const char* relative_path);
    void deleteTexture();
    
    GLuint getTexture();


};

#endif /* gles2_texture_h */
