#include "RawImage.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

RawImage::RawImage() {
    this->width = 0;
    this->height = 0;
    this->size = 0;
    this->color_format = NULL;
    this->data = NULL;
}

RawImage::RawImage(int width, int height, int size, int color_format, void* data){
    this->width = width;
    this->height = height;
    this->size = size;
    this->color_format = color_format;
    this->data = data;
}

RawImage::RawImage(const RawImage& v){
    this->width = v.width;
    this->height = v.height;
    this->size = v.size;
    this->color_format = v.color_format;
    this->data = v.data;
}

RawImage& RawImage::operator = ( const RawImage& v )
{
    this->width = v.width;
    this->height = v.height;
    this->size = v.size;
    this->color_format = v.color_format;
    this->data = v.data;

    return *this;
}

void RawImage::copy ( const RawImage& v )
{
    this->width = v.width;
    this->height = v.height;
    this->size = v.size;
    this->color_format = v.color_format;

    this->data = (void *)malloc(this->size);
    memcpy((void*)this->data, (void*)v.data, this->size);
}

RawImage::~RawImage() {
}

void RawImage::releaseImageData(){
    free((void*)data);
}

int RawImage::getWidth(){
    return width;
}

int RawImage::getHeight(){
    return height;
}

int RawImage::getSize(){
    return size;
}

int RawImage::getColorFormat(){
    return color_format;
}

void* RawImage::getData(){
    return data;
}
