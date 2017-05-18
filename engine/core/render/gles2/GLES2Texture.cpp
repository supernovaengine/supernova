#include "GLES2Texture.h"
#include "image/ColorType.h"


GLES2Texture::GLES2Texture(){
    
}

GLenum GLES2Texture::getGlColorFormat(const int color_format) {

    switch (color_format) {
        case S_COLOR_GRAY:
            return GL_LUMINANCE;
        case S_COLOR_RGB_ALPHA:
            return GL_RGBA;
        case S_COLOR_RGB:
            return GL_RGB;
        case S_COLOR_GRAY_ALPHA:
            return GL_LUMINANCE_ALPHA;
        case S_COLOR_ALPHA:
            return GL_ALPHA;
    }

    return 0;
}

void GLES2Texture::assignTexture(const GLenum target, const GLsizei width, const GLsizei height, const GLenum type, const GLvoid* pixels) {
    
    glTexImage2D(target, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, pixels);
    
}

void GLES2Texture::loadTexture(TextureFile* texturefile) {
    GLuint texture_object_id;
    
    glGenTextures(1, &texture_object_id);
    //assert(texture_object_id != 0);
    
    glBindTexture(GL_TEXTURE_2D, texture_object_id);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    assignTexture(GL_TEXTURE_2D, texturefile->getWidth(), texturefile->getHeight(), getGlColorFormat(texturefile->getColorFormat()), texturefile->getData());
    
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    textureType = GL_TEXTURE_2D;
    
    gTexture = texture_object_id;
}

void GLES2Texture::loadTextureCube(std::vector<TextureFile*> texturefiles){
    GLuint texture_object_id;
    
    glGenTextures(1, &texture_object_id);
    //assert(texture_object_id != 0);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_object_id);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
    
    for(GLuint i = 0; i < texturefiles.size(); i++){
        int colorFormat = getGlColorFormat(texturefiles[i]->getColorFormat());
        assignTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, texturefiles[i]->getWidth(), texturefiles[i]->getHeight(), colorFormat, texturefiles[i]->getData());
    }
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    textureType = GL_TEXTURE_CUBE_MAP;
    
    gTexture = texture_object_id;
}

void GLES2Texture::deleteTexture(){
    glDeleteTextures(1, &gTexture);
}

GLuint GLES2Texture::getTexture(){
    return gTexture;
}

GLenum GLES2Texture::getTextureType(){
    return textureType;
}
