#ifndef gles2texture_h
#define gles2texture_h

#include "GLES2Header.h"
#include "image/TextureLoader.h"

class GLES2Texture{

private:
    static GLenum getGlColorFormat(const int color_format);

public:


    static GLuint loadAssetIntoTexture(const char* relative_path);

    static GLuint loadTexture(const GLsizei width, const GLsizei height, const GLenum type, const GLvoid* pixels);


};

#endif /* gles2_texture_h */
