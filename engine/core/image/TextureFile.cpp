#include "TextureFile.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

TextureFile::TextureFile() {
    this->width = 0;
    this->height = 0;
    this->size = 0;
    this->color_format = 0;
    this->data = NULL;
}

TextureFile::TextureFile(int width, int height, int size, int color_format, void* data){
    this->width = width;
    this->height = height;
    this->size = size;
    this->color_format = color_format;
    this->data = data;
}

TextureFile::TextureFile(const TextureFile& v){
    this->width = v.width;
    this->height = v.height;
    this->size = v.size;
    this->color_format = v.color_format;
    this->data = v.data;
}

TextureFile& TextureFile::operator = ( const TextureFile& v )
{
    this->width = v.width;
    this->height = v.height;
    this->size = v.size;
    this->color_format = v.color_format;
    this->data = v.data;

    return *this;
}

void TextureFile::copy ( const TextureFile& v )
{
    this->width = v.width;
    this->height = v.height;
    this->size = v.size;
    this->color_format = v.color_format;

    this->data = (void *)malloc(this->size);
    memcpy((void*)this->data, (void*)v.data, this->size);
}

TextureFile::~TextureFile() {

}

void TextureFile::releaseImageData(){
    free((void*)data);
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

void* TextureFile::getData(){
    return data;
}
