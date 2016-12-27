#include "TextureFile.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

TextureFile::TextureFile() {
    this->width = 0;
    this->height = 0;
    this->size = 0;
    this->color_format = 0;
    this->bitsPerPixel = 0;
    this->data = NULL;
}

TextureFile::TextureFile(int width, int height, int size, int color_format, int bitsPerPixel, void* data){
    this->width = width;
    this->height = height;
    this->size = size;
    this->color_format = color_format;
    this->bitsPerPixel = bitsPerPixel;
    this->data = data;
}

TextureFile::TextureFile(const TextureFile& v){
    this->copy(v);
}

TextureFile& TextureFile::operator = ( const TextureFile& v )
{
    this->copy(v);

    return *this;
}

void TextureFile::copy ( const TextureFile& v )
{
    this->width = v.width;
    this->height = v.height;
    this->size = v.size;
    this->color_format = v.color_format;
    this->bitsPerPixel = v.bitsPerPixel;

    this->data = (void *)malloc(this->size);
    memcpy((void*)this->data, (void*)v.data, this->size);
}

TextureFile::~TextureFile() {
    releaseImageData();
}

void TextureFile::releaseImageData(){
    free((void*)data);
}

void TextureFile::crop(int offsetX, int offsetY, int newWidth, int newHeight){
    
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

void TextureFile::flipVertical(){
    
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

int TextureFile::getWidth(){
    return width;
}

int TextureFile::getHeight(){
    return height;
}

int TextureFile::getSize(){
    return size;
}

int TextureFile::getColorFormat(){
    return color_format;
}

int TextureFile::getBitsPerPixel(){
    return bitsPerPixel;
}

void* TextureFile::getData(){
    return data;
}
