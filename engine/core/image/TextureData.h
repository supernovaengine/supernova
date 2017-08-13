#ifndef texturedata_h
#define texturedata_h

namespace Supernova {

    class TextureData {
    private:

        int width;
        int height;
        int size;
        int color_format;
        int bitsPerPixel;
        void* data;
        
        int getNearestPowerOfTwo(int size);

    public:

        TextureData();
        TextureData(int width, int height, int size, int color_format, int bitsPerPixel, void* data);
        TextureData(const TextureData& v);
        TextureData& operator = ( const TextureData& v );
        virtual ~TextureData();
        void copy(const TextureData& v);

        void releaseImageData();
        void crop(int offsetX, int offsetY, int newWidth, int newHeight);
        void resample(int newWidth, int newHeight);
        void resamplePowerOfTwo();
        void flipVertical();

        int getWidth();
        int getHeight();
        int getSize();
        int getColorFormat();
        int getBitsPerPixel();
        void* getData();

    };
    
}


#endif /* texturedata_h */
