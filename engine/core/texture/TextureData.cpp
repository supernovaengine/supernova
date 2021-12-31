#include "TextureData.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "io/Data.h"
#include "stb_image.h"
#include "Log.h"
#include "Texture.h"
#include "Engine.h"

using namespace Supernova;

TextureData::TextureData() {
    this->width = 0;
    this->height = 0;
    this->size = 0;
    this->color_format = ColorFormat::RGBA;
    this->channels = 0;
    this->data = NULL;

    this->transparent = false;
    
    this->dataOwned = false;
}

TextureData::TextureData(int width, int height, unsigned int size, ColorFormat color_format, int channels, void* data){
    this->width = width;
    this->height = height;
    this->size = size;
    this->color_format = color_format;
    this->channels = channels;
    this->data = data;

    this->transparent = false;
    
    this->dataOwned = false;
}

TextureData::TextureData(const TextureData& v){
    this->copy(v);
}

TextureData& TextureData::operator = ( const TextureData& v ){
    this->copy(v);

    return *this;
}

bool TextureData::loadTextureFromFile(const char* filename) {
    
    Data filedata;
    
    int res = filedata.open(filename);
    
    if (res==FileErrors::FILE_NOT_FOUND){
        Log::Error("Texture file not found: %s", filename);
        return false;
    }
    if (res==FileErrors::INVALID_PARAMETER){
        Log::Error("Texture file path is invalid: %s", filename);
        return false;
    }
    filedata.seek(0);
    
    //----- Start std_image read texture
    stbi_info_from_memory((stbi_uc const *)filedata.getMemPtr(), filedata.length(), &width, &height, &channels);

    //Renders (not GL) only support 8bpp and 32bpp
    int desired_channels = 1;
    color_format = ColorFormat::RED;

    if (channels != 1){
        desired_channels = 4;
        color_format = ColorFormat::RGBA;
    }

    data = stbi_load_from_memory((stbi_uc const *)filedata.getMemPtr(), filedata.length(), &width, &height, &channels, desired_channels);

    if (!data){
        Log::Error("Error loading texture file (%s): %s", filename, stbi_failure_reason());
        return false;
    }

    //Considering one byte per channel
    size = width * height * desired_channels; //in bytes
    //----- End std_image read texture

    if (Engine::isAutomaticTransparency){
        transparent = hasAlpha();
    }
    
    return true;

}

void TextureData::copy ( const TextureData& v ){
    this->width = v.width;
    this->height = v.height;
    this->size = v.size;
    this->color_format = v.color_format;
    this->channels = v.channels;

    this->dataOwned = v.dataOwned;

    this->transparent = v.transparent;

    this->data = v.data;
}

TextureData::~TextureData() {
    if (dataOwned)
        releaseImageData();
}

void TextureData::releaseImageData(){
    free((void*)data);
}

int TextureData::getNearestPowerOfTwo(int size){
    int i;
    for(i = 31; i >= 0; i--)
        if ((size - 1) & (1<<i))
            break;
    return (1<<(i + 1));
}

bool TextureData::hasAlpha(){
    if (channels == 4){
        for(int y = 0; y < height; y++){
            for(int x = 0; x < width; x++){
                int pixel = (y * (width * channels)) + (x * channels);
                if (((unsigned char*)data)[pixel + 3] != 255)
                    return true;
            }
        }
    }
    return false;
}

void TextureData::crop(int offsetX, int offsetY, int newWidth, int newHeight){
    
    int rowsize = width * channels;
    int newRowsize = newWidth * channels;
    int bufsize = newWidth * newHeight * channels;
    
    unsigned char* newData = new unsigned char[bufsize];
    
    int row_cnt;
    long off1 = 0;
    long off2 = 0;
    long off3 = offsetX*channels;
    
    for (row_cnt=offsetY;row_cnt<offsetY+(newHeight);row_cnt++) {
        off1=row_cnt*rowsize;
        off2=(row_cnt-offsetY)*newRowsize;
        
        memcpy(newData+off2,(unsigned char*)data+off1+off3,newRowsize);
    }

    free((void*)data);
    
    width = newWidth;
    height = newHeight;
    size = bufsize;
    data = newData;
}

void TextureData::resamplePowerOfTwo(){
    resample(getNearestPowerOfTwo(width), getNearestPowerOfTwo(height));
}

void TextureData::resample(int newWidth, int newHeight){

    if ((newWidth != width) || (newHeight != height)){

        int bufsize = newWidth * newHeight * channels;
        unsigned char* newData = new unsigned char [bufsize];
    
        double scaleWidth =  (double)newWidth / (double)width;
        double scaleHeight = (double)newHeight / (double)height;
   
        for(int cy = 0; cy < newHeight; cy++){
            for(int cx = 0; cx < newWidth; cx++){
                
                int pixel = (cy * (newWidth*channels)) + (cx*channels);
                int nearestMatch =  (((int)(cy / scaleHeight) * (width*channels)) + ((int)(cx / scaleWidth) * channels) );
            
                for (int color = 0; color < channels; color++){
                    newData[pixel + color] =  ((unsigned char*)data)[nearestMatch + color];
                }
            
            }
        }

        free((void*)data);
    
        width = newWidth;
        height = newHeight;
        size = bufsize;
        data = newData;
        
    }

}

void TextureData::fitPowerOfTwo(){
    fitSize(getNearestPowerOfTwo(width), getNearestPowerOfTwo(height));
}

void TextureData::fitSize(int newWidth, int newHeight){
    
    if ((newWidth != width) || (newHeight != height)){

        int bufsize = newWidth * newHeight * channels;
        unsigned char* newData = new unsigned char [bufsize];
        
        int xOffset = 0;
        int yOffset = 0;
        
        for( unsigned int i = 0; i < bufsize; ++i )
        {
            newData[i] = 0;
        }
        
        for( unsigned int i = 0; i < height; ++i )
        {
            memcpy( (unsigned char*)newData + channels * ( newWidth * ( yOffset + i ) + xOffset ), (unsigned char*)data + width * i * channels, width * channels );
        }
        
        free((void*)data);
        
        width = newWidth;
        height = newHeight;
        size = bufsize;
        data = newData;
        
    }
    
}

void TextureData::flipVertical(){
    
    int bufsize = width * channels;
    
    unsigned char* tb1 = new unsigned char[bufsize];
    unsigned char* tb2 = new unsigned char[bufsize];
    
    int row_cnt;
    long off1 = 0;
    long off2 = 0;
    
    for (row_cnt=0;row_cnt<(height+1)/2;row_cnt++) {
        off1=row_cnt*bufsize;
        off2=((height-1)-row_cnt)*bufsize;
        
        memcpy(tb1,(unsigned char*)data+off1,bufsize);
        memcpy(tb2,(unsigned char*)data+off2,bufsize);
        memcpy((unsigned char*)data+off1,tb2,bufsize);
        memcpy((unsigned char*)data+off2,tb1,bufsize);
    }
    
    delete [] tb1;
    delete [] tb2;
    
}

unsigned char TextureData::getColorComponent(int x, int y, int color){
    return ((unsigned char*)data)[((x + y*width)*channels)+color];
}

void TextureData::setDataOwned(bool dataOwned){
    this->dataOwned = dataOwned;
}

int TextureData::getWidth(){
    return width;
}

int TextureData::getHeight(){
    return height;
}

unsigned int TextureData::getSize(){
    return size;
}

ColorFormat TextureData::getColorFormat(){
    return color_format;
}

int TextureData::getChannels(){
    return channels;
}

void* TextureData::getData(){
    return data;
}

bool TextureData::isTransparent(){
    return transparent;
}
