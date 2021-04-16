#include "SokolTexture.h"

#include "Log.h"

using namespace Supernova;

SokolTexture::SokolTexture(){
    image.id = SG_INVALID_ID;
}

SokolTexture::SokolTexture(const SokolTexture& rhs): image(rhs.image) {}

SokolTexture& SokolTexture::operator=(const SokolTexture& rhs){ 
    image = rhs.image; 
    return *this;
}

sg_image_type SokolTexture::getTextureType(TextureType textureType){
    if (textureType == TextureType::TEXTURE_2D){
        return SG_IMAGETYPE_2D;
    }else if (textureType == TextureType::TEXTURE_CUBE){
        return SG_IMAGETYPE_CUBE;
    }

    return SG_IMAGETYPE_2D;
}

bool SokolTexture::createTexture(std::string label, int width, int height, ColorFormat colorFormat, TextureType type, int numFaces, TextureDataSize* texData){

    sg_pixel_format pixelFormat;
    if (colorFormat == ColorFormat::RGBA){
        pixelFormat = SG_PIXELFORMAT_RGBA8;
    }else if (colorFormat == ColorFormat::GRAY){
        pixelFormat = SG_PIXELFORMAT_R8;
    }else{
        Log::Error("Renders only support 8bpp and 32bpp textures");
    }

    sg_image_desc image_desc = {
        .type = getTextureType(type),
        .width = width,
        .height = height,
        .pixel_format = pixelFormat,
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
        .label = label.c_str()
    };

    for (int f = 0; f < numFaces; f++){
        image_desc.data.subimage[f][0].ptr = texData[f].data;
        image_desc.data.subimage[f][0].size = (size_t)texData[f].size;
    }

    image = sg_make_image(&image_desc);

    if (image.id != SG_INVALID_ID)
        return true;

    return false;
}

void SokolTexture::destroyTexture(){
    if (image.id != SG_INVALID_ID && sg_isvalid()){
        sg_destroy_image(image);
    }

    image.id = SG_INVALID_ID;
}

sg_image SokolTexture::get(){
    return image;
}