#include "GLES2Texture.h"
#include "image/ColorType.h"
#include "Engine.h"
#include "Scene.h"
#include "platform/Log.h"
#include "GLES2Util.h"

using namespace Supernova;

GLES2Texture::GLES2Texture(){
    frameBuffer = 0;
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

void GLES2Texture::loadTexture(TextureData* texturedata) {
    TextureRender::loadTexture(texturedata);
    
    GLuint texture_object_id;
    
    glGenTextures(1, &texture_object_id);
    //assert(texture_object_id != 0);
    
    glBindTexture(GL_TEXTURE_2D, texture_object_id);
    
    if (Engine::isNearestScaleTexture()){
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }else{
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    
    assignTexture(GL_TEXTURE_2D, texturedata->getWidth(), texturedata->getHeight(), getGlColorFormat(texturedata->getColorFormat()), texturedata->getData());
    
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    textureType = GL_TEXTURE_2D;
    
    gTexture = texture_object_id;
}

void GLES2Texture::loadTextureCube(std::vector<TextureData*> texturesdata){
    TextureRender::loadTextureCube(texturesdata);
    
    GLuint texture_object_id;
    
    glGenTextures(1, &texture_object_id);
    //assert(texture_object_id != 0);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_object_id);
    
    if (Engine::isNearestScaleTexture()){
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }else{
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
    
    for(GLuint i = 0; i < texturesdata.size(); i++){
        int colorFormat = getGlColorFormat(texturesdata[i]->getColorFormat());
        assignTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, texturesdata[i]->getWidth(), texturesdata[i]->getHeight(), colorFormat, texturesdata[i]->getData());
    }
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    textureType = GL_TEXTURE_CUBE_MAP;
    
    gTexture = texture_object_id;
}

void GLES2Texture::loadTextureFrame(int width, int height){
    TextureRender::loadTextureFrame(width, height);

    const char* extensions = (char*)glGetString(GL_EXTENSIONS);
    
    if(!strstr(extensions, "depth_texture")){
        Log::Error(LOG_TAG,"This device has no support to depth_texture");
    }else{
        
        GLuint FramebufferName = 0;
        glGenFramebuffers(1, &FramebufferName);
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

        GLuint depthTexture;
        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        
        GLuint renderbuffer;
        glGenRenderbuffers(1, &renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthTexture, 0);

        //glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 0);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    
        GLES2Util::checkGlError("Error on framebuffer");
        
       // Log::Verbose(LOG_TAG,"Loading framebuffer");

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            Log::Error(LOG_TAG,"Error on creating framebuffer");
        
        if (Engine::getPlatform()==S_PLATFORM_IOS){
            glBindFramebuffer(GL_FRAMEBUFFER, 1);
        }else{
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        
        textureType = GL_TEXTURE_2D;
        gTexture = depthTexture;
        frameBuffer = FramebufferName;
    
    }

}

void GLES2Texture::initTextureFrame(){
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
}

void GLES2Texture::endTextureFrame(){
    if (Engine::getPlatform()==S_PLATFORM_IOS){
        glBindFramebuffer(GL_FRAMEBUFFER, 1);
    }else{
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void GLES2Texture::deleteTexture(){
    
    glDeleteTextures(1, &gTexture);
    
    if (frameBuffer > 0)
        glDeleteFramebuffers(1, &frameBuffer);
    
    TextureRender::deleteTexture();
}

GLuint GLES2Texture::getTexture(){
    return gTexture;
}

GLenum GLES2Texture::getTextureType(){
    return textureType;
}
