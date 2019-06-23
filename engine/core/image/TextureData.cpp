#include "TextureData.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

using namespace Supernova;

TextureData::TextureData() {
    this->width = 0;
    this->height = 0;
    this->size = 0;
    this->color_format = 0;
    this->bitsPerPixel = 0;
    this->data = NULL;
}

TextureData::TextureData(int width, int height, unsigned int size, int color_format, int bitsPerPixel, void* data){
    this->width = width;
    this->height = height;
    this->size = size;
    this->color_format = color_format;
    this->bitsPerPixel = bitsPerPixel;
    this->data = data;
}

TextureData::TextureData(const TextureData& v){
    this->copy(v);
}

TextureData& TextureData::operator = ( const TextureData& v ){
    this->copy(v);

    return *this;
}

void TextureData::copy ( const TextureData& v ){
    this->width = v.width;
    this->height = v.height;
    this->size = v.size;
    this->color_format = v.color_format;
    this->bitsPerPixel = v.bitsPerPixel;

    this->data = (void *)malloc(this->size);
    memcpy((void*)this->data, (void*)v.data, this->size);
}

TextureData::~TextureData() {
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

void TextureData::crop(int offsetX, int offsetY, int newWidth, int newHeight){
    
    int rowsize = width * (bitsPerPixel / 8);
    int newRowsize = newWidth * (bitsPerPixel / 8);
    int bufsize = newWidth * newHeight * (bitsPerPixel / 8);
    
    unsigned char* newData = new unsigned char[bufsize];
    
    int row_cnt;
    long off1=0;
    long off2=0;
    long off3=offsetX*(bitsPerPixel / 8);
    
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
    
        int channels = (bitsPerPixel / 8);
        int bufsize = newWidth * newHeight * channels;
        unsigned char* newData = new unsigned char [bufsize];
    
        double scaleWidth =  (double)newWidth / (double)width;
        double scaleHeight = (double)newHeight / (double)height;
   
        for(int cy = 0; cy < newHeight; cy++){
            for(int cx = 0; cx < newWidth; cx++){
                
                int pixel = (cy * (newWidth *channels)) + (cx*channels);
                int nearestMatch =  (((int)(cy / scaleHeight) * (width *channels)) + ((int)(cx / scaleWidth) *channels) );
            
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
        
        int channels = (bitsPerPixel / 8);
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
    
    int bufsize=width * (bitsPerPixel / 8);
    
    unsigned char* tb1 = new unsigned char[bufsize];
    unsigned char* tb2 = new unsigned char[bufsize];
    
    int row_cnt;
    long off1=0;
    long off2=0;
    
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

int TextureData::getWidth(){
    return width;
}

int TextureData::getHeight(){
    return height;
}

unsigned int TextureData::getSize(){
    return size;
}

int TextureData::getColorFormat(){
    return color_format;
}

int TextureData::getBitsPerPixel(){
    return bitsPerPixel;
}

void* TextureData::getData(){
    return data;
}
