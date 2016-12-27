
#ifndef Image_h
#define Image_h

#include "Mesh.h"

class Image: public Mesh {
    
private:
    int width;
    int height;
    
    void createVertices();
    
public:
    Image();
    Image(int width, int height);
    Image(std::string image_path);
    virtual ~Image();
    
    void setSizes(int width, int height);
    
    void setWidth(int width);
    int getWidth();
    
    void setHeight(int height);
    int getHeight();
    
    bool load();

};

#endif /* Image_h */
