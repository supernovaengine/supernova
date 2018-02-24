#include "STBImageReader.h"

#include "stb_image.h"
#include "Texture.h"

using namespace Supernova;

TextureData* STBImageReader::getRawImage(FileData* filedata){
    
    //stbi_set_flip_vertically_on_load(true);
    
    int x,y,channels;
    
    unsigned char *data = stbi_load_from_memory((stbi_uc const *)filedata->getMemPtr(), filedata->length(), &x, &y, &channels, 0);
    
    int type = S_COLOR_GRAY;
    if(channels == 2) type = S_COLOR_GRAY_ALPHA;
    if(channels == 3) type = S_COLOR_RGB;
    if(channels == 4) type = S_COLOR_RGB_ALPHA;
    
    int size = x * y * channels; //in bytes
    
    TextureData* textureData  = new TextureData((int)x, (int)y, (int)size, type, channels * 8, (void*)data);
    //textureFile->flipVertical();
    //stbi_image_free(data);
    
    return textureData;
}
