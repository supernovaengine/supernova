#ifndef gles2texture_h
#define gles2texture_h

#include "GLES2Header.h"
#include "render/TextureRender.h"

class GLES2Texture : public TextureRender{

private:
    GLuint gTexture;
    GLenum textureType;
    
    GLenum getGlColorFormat(const int color_format);
    void assignTexture(const GLenum target, const GLsizei width, const GLsizei height, const GLenum type, const GLvoid* pixels);

public:
    
    GLES2Texture();

    void loadTexture(TextureFile* texturefile);
    void loadTextureCube(std::vector<TextureFile*> texturefiles);
    void deleteTexture();
    
    void setTexture(GLuint texture);
    GLuint getTexture();
    GLenum getTextureType();

};

#endif /* gles2_texture_h */
