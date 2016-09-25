#include "ImageReader.h"


ImageReader::~ImageReader(){

}


void ImageReader::flipVertical(unsigned char* inbuf, int width, int height, int bitsPerPixel){

    int bufsize=width * (bitsPerPixel / 8);
    
    unsigned char* tb1 = new unsigned char[bufsize];
    unsigned char* tb2 = new unsigned char[bufsize];
    
    int row_cnt;
    long off1=0;
    long off2=0;
    
    for (row_cnt=0;row_cnt<(height+1)/2;row_cnt++) {
        off1=row_cnt*bufsize;
        off2=((height-1)-row_cnt)*bufsize;
        
        memcpy(tb1,inbuf+off1,bufsize);
        memcpy(tb2,inbuf+off2,bufsize);
        memcpy(inbuf+off1,tb2,bufsize);
        memcpy(inbuf+off2,tb1,bufsize);
    }
    
    delete [] tb1;
    delete [] tb2;
    
}
