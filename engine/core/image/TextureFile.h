#ifndef texturefile_h
#define texturefile_h

class TextureFile {
private:

    int width;
    int height;
    int size;
    int color_format;
    void* data;

public:

    TextureFile();
    TextureFile(int width, int height, int size, int color_format, void* data);
    TextureFile(const TextureFile& v);
    TextureFile& operator = ( const TextureFile& v );
    virtual ~TextureFile();
    void copy(const TextureFile& v);

    void releaseImageData();

    int getWidth();
    int getHeight();
    int getSize();
    int getColorFormat();
    void* getData();

};


#endif /* texturefile_h */
