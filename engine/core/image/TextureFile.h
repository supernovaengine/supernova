#ifndef texturefile_h
#define texturefile_h

class TextureFile {
private:

    int width;
    int height;
    int size;
    int color_format;
    int bitsPerPixel;
    void* data;
    
    int getNearestPowerOfTwo(int size);

public:

    TextureFile();
    TextureFile(int width, int height, int size, int color_format, int bitsPerPixel, void* data);
    TextureFile(const TextureFile& v);
    TextureFile& operator = ( const TextureFile& v );
    virtual ~TextureFile();
    void copy(const TextureFile& v);

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


#endif /* texturefile_h */
