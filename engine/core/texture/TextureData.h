//
// (c) 2024 Eduardo Doria.
//

#ifndef texturedata_h
#define texturedata_h

#include "render/Render.h"
#include "io/Data.h"

namespace Supernova {

    class TextureData {
    private:

        int width;
        int height;

        int originalWidth;
        int originalHeight;

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
        bool operator == ( const TextureData& v ) const;
        bool operator != ( const TextureData& v ) const;
        virtual ~TextureData();
        void copy(const TextureData& v);
        
        bool loadTexture(Data* filedata);
        bool loadTextureFromFile(const char* filename);
        bool loadTextureFromMemory(unsigned char* data, unsigned int dataLength);

        void releaseImageData();
        
        bool hasAlpha();

        void resizePowerOfTwo();
        void fitPowerOfTwo();

        void resize(int newWidth, int newHeight);
        void crop(int xOffset, int yOffset, int newWidth, int newHeight);
        void fitSize(int xOffset, int yOffset, int newWidth, int newHeight);

        void flipVertical();

        unsigned char getColorComponent(int x, int y, int color);
        
        void setDataOwned(bool dataOwned);

        int getWidth();
        int getHeight();
        int getOriginalWidth();
        int getOriginalHeight();
        unsigned int getSize();
        ColorFormat getColorFormat();
        int getChannels();
        void* getData();

        bool isTransparent();

        int getNearestPowerOfTwo();

        // render callback clean function
        static void cleanupTexture(void* data);
    };
    
}


#endif /* texturedata_h */
