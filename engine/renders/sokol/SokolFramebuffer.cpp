#include "SokolFramebuffer.h"
#include "Log.h"

using namespace Supernova;

SokolFramebuffer::SokolFramebuffer(){
    for (int i = 0; i < 6; i++){
        pass[i].id = SG_INVALID_ID;
    }
}

SokolFramebuffer::SokolFramebuffer(const SokolFramebuffer& rhs){
    for (int i = 0; i < 6; i++){
        pass[i] = rhs.pass[i];
    }
}

SokolFramebuffer& SokolFramebuffer::operator=(const SokolFramebuffer& rhs){
    for (int i = 0; i < 6; i++){
        pass[i] = rhs.pass[i];
    }
    return *this;
}

bool SokolFramebuffer::createFramebuffer(TextureType textureType, int width, int height, TextureFilter minFilter, TextureFilter magFilter, bool shadowMap){
    if ((textureType != TextureType::TEXTURE_2D) && (textureType != TextureType::TEXTURE_CUBE)){
        Log::error("Framebuffer texture type must be 2D or CUBE");
        return false;
    }
    colorTexture.createFramebufferTexture(textureType, false, shadowMap, width, height, minFilter, magFilter);
    depthTexture.createFramebufferTexture(TextureType::TEXTURE_2D, true, shadowMap, width, height, minFilter, magFilter);

    size_t faces = (textureType == TextureType::TEXTURE_CUBE)? 6 : 1;

    for (int i = 0; i < faces; i++){
        sg_pass_desc pass_desc = {0};
        pass_desc.color_attachments[0].image = colorTexture.backend.get();
        pass_desc.color_attachments[0].slice = i;
        pass_desc.depth_stencil_attachment.image = depthTexture.backend.get();
        pass_desc.label = "shadow-map-pass";

        pass[i] = sg_make_pass(&pass_desc);
    }

    return isCreated();
}

void SokolFramebuffer::destroyFramebuffer(){
    if (pass[0].id != SG_INVALID_ID && sg_isvalid()){
        for (int i = 0; i < 6; i++){
            sg_destroy_pass(pass[i]);
        }
        colorTexture.destroyTexture();
        depthTexture.destroyTexture();
    }

    for (int i = 0; i < 6; i++){
        pass[i].id = SG_INVALID_ID;
    }
}

bool SokolFramebuffer::isCreated(){
    if (pass[0].id != SG_INVALID_ID)
        return true;

    return false;
}

TextureRender& SokolFramebuffer::getColorTexture(){
    return colorTexture;
}

sg_pass SokolFramebuffer::get(size_t face){
    return pass[face];
}
