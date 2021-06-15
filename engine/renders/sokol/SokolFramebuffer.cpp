#include "SokolFramebuffer.h"
#include "Log.h"

using namespace Supernova;

SokolFramebuffer::SokolFramebuffer(){
    pass.id = SG_INVALID_ID;
}

SokolFramebuffer::SokolFramebuffer(const SokolFramebuffer& rhs): pass(rhs.pass) {}

SokolFramebuffer& SokolFramebuffer::operator=(const SokolFramebuffer& rhs){
    pass = rhs.pass;
    return *this;
}

bool SokolFramebuffer::createFramebuffer(int width, int height){
    colorTexture.createShadowMapColorTexture(width, height);

    sg_image_desc img_desc = {0};
    img_desc.render_target = true;
    img_desc.width = width;
    img_desc.height = height;
    img_desc.min_filter = SG_FILTER_LINEAR;
    img_desc.mag_filter = SG_FILTER_LINEAR;
    img_desc.sample_count = 1;
    img_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
    img_desc.label = "shadow-map-depth-image";

    sg_image depth_img = sg_make_image(&img_desc);

    sg_pass_desc pass_desc = {0};
    pass_desc.color_attachments[0].image = colorTexture.backend.get();
    pass_desc.depth_stencil_attachment.image = depth_img;
    pass_desc.label = "shadow-map-pass";

    pass = sg_make_pass(&pass_desc);

    if (pass.id != SG_INVALID_ID)
        return true;

    return false;
}

void SokolFramebuffer::destroyFramebuffer(){
    if (pass.id != SG_INVALID_ID && sg_isvalid()){
        sg_destroy_pass(pass);
    }

    pass.id = SG_INVALID_ID;
}

TextureRender* SokolFramebuffer::getColorTexture(){
    return &colorTexture;
}

sg_pass SokolFramebuffer::get(){
    return pass;
}