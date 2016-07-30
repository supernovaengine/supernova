#ifndef rawimage_h
#define rawimage_h

class RawImage {
private:

    int width;
    int height;
    int size;
    int color_format;
    void* data;

public:

    RawImage();
    RawImage(int width, int height, int size, int color_format, void* data);
    RawImage(const RawImage& v);
    RawImage& operator = ( const RawImage& v );
    virtual ~RawImage();
    void copy(const RawImage& v);

    void releaseImageData();

    int getWidth();
    int getHeight();
    int getSize();
    int getColorFormat();
    void* getData();

};


#endif /* rawimage_h */
