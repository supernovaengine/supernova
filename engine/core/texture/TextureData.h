#ifndef texturedata_h
#define texturedata_h

#include "render/Render.h"

namespace Supernova {

    class TextureData {
    private:

        int width;
        int height;
        unsigned int size; //in bytes
        ColorFormat color_format;
        int channels;
        void* data;

        bool transparent;
        
        bool dataOwned;
        
        int getNearestPowerOfTwo(int size);

    public:

        TextureData();
        TextureData(int width, int height, unsigned int size, ColorFormat color_format, int channels, void* data);
        TextureData(const TextureData& v);
        TextureData& operator = ( const TextureData& v );
        virtual ~TextureData();
        void copy(const TextureData& v);
        
        bool loadTextureFromFile(const char* filename);

        void releaseImageData();
        
        bool hasAlpha();
        void crop(int offsetX, int offsetY, int newWidth, int newHeight);
        void resamplePowerOfTwo();
        void fitPowerOfTwo();
        void resample(int newWidth, int newHeight);
        void fitSize(int newWidth, int newHeight);
        void flipVertical();

        unsigned char getColorComponent(int x, int y, int color);
        
        void setDataOwned(bool dataOwned);

        int getWidth();
        int getHeight();
        unsigned int getSize();
        ColorFormat getColorFormat();
        int getChannels();
        void* getData();

        bool isTransparent();
    };
    
}


#endif /* texturedata_h */
