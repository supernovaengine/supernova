//
// (c) 2024 Eduardo Doria.
//

#include "SokolFramebuffer.h"
#include "Log.h"
#include "SokolCmdQueue.h"
#include "Engine.h"

using namespace Supernova;

SokolFramebuffer::SokolFramebuffer(){
    for (int i = 0; i < 6; i++){
        attachments[i].id = SG_INVALID_ID;
    }
}

SokolFramebuffer::SokolFramebuffer(const SokolFramebuffer& rhs){
    for (int i = 0; i < 6; i++){
        attachments[i] = rhs.attachments[i];
    }
}

SokolFramebuffer& SokolFramebuffer::operator=(const SokolFramebuffer& rhs){
    for (int i = 0; i < 6; i++){
        attachments[i] = rhs.attachments[i];
    }
    return *this;
}

bool SokolFramebuffer::createFramebuffer(TextureType textureType, int width, int height, TextureFilter minFilter, TextureFilter magFilter, TextureWrap wrapU, TextureWrap wrapV, bool shadowMap){
    if ((textureType != TextureType::TEXTURE_2D) && (textureType != TextureType::TEXTURE_CUBE)){
        Log::error("Framebuffer texture type must be 2D or CUBE");
        return false;
    }
    colorTexture.createFramebufferTexture(textureType, false, shadowMap, width, height, minFilter, magFilter, wrapU, wrapV);
    depthTexture.createFramebufferTexture(TextureType::TEXTURE_2D, true, shadowMap, width, height, minFilter, magFilter, wrapU, wrapV);

    size_t faces = (textureType == TextureType::TEXTURE_CUBE)? 6 : 1;

    for (int i = 0; i < faces; i++){
        sg_attachments_desc attachments_desc = {0};
        attachments_desc.colors[0].image = colorTexture.backend.get();
        attachments_desc.colors[0].slice = i;
        attachments_desc.depth_stencil.image = depthTexture.backend.get();
        attachments_desc.label = "shadow-map-pass";

        if (Engine::isAsyncThread()){
            attachments[i] = SokolCmdQueue::add_command_make_attachments(attachments_desc);
        }else{
            attachments[i] = sg_make_attachments(attachments_desc);
        }
    }

    return isCreated();
}

void SokolFramebuffer::destroyFramebuffer(){
    if (attachments[0].id != SG_INVALID_ID && sg_isvalid()){
        for (int i = 0; i < 6; i++){
            if (Engine::isAsyncThread()){
                SokolCmdQueue::add_command_destroy_attachments(attachments[i]);
            }else{
                sg_destroy_attachments(attachments[i]);
            }
        }
        colorTexture.destroyTexture();
        depthTexture.destroyTexture();
    }

    for (int i = 0; i < 6; i++){
        attachments[i].id = SG_INVALID_ID;
    }
}

bool SokolFramebuffer::isCreated(){
    if (attachments[0].id != SG_INVALID_ID)
        return true;

    return false;
}

TextureRender& SokolFramebuffer::getColorTexture(){
    return colorTexture;
}

TextureRender& SokolFramebuffer::getDepthTexture(){
    return depthTexture;
}

sg_attachments SokolFramebuffer::get(size_t face){
    return attachments[face];
}
