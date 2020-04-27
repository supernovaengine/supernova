#ifndef texturedata_h
#define texturedata_h

namespace Supernova {

    class TextureData {
    private:

        int width;
        int height;
        unsigned int size; //in bytes
        int color_format;
        int channels;
        void* data;
        
        int getNearestPowerOfTwo(int size);

    public:

        TextureData();
        TextureData(int width, int height, unsigned int size, int color_format, int channels, void* data);
        TextureData(const TextureData& v);
        TextureData& operator = ( const TextureData& v );
        virtual ~TextureData();
        void copy(const TextureData& v);

        void releaseImageData();
        
        void crop(int offsetX, int offsetY, int newWidth, int newHeight);
        void resamplePowerOfTwo();
        void fitPowerOfTwo();
        void resample(int newWidth, int newHeight);
        void fitSize(int newWidth, int newHeight);
        void flipVertical();

        unsigned char getColorComponent(int x, int y, int color);

        int getWidth();
        int getHeight();
        unsigned int getSize();
        int getColorFormat();
        int getChannels();
        void* getData();

    };
    
}


#endif /* texturedata_h */
