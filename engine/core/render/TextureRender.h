
#ifndef TextureRender_h
#define TextureRender_h

class TextureRender {
    
public:
    
    inline virtual ~TextureRender(){}
    
    virtual void loadTexture(int width, int height, int type, void* data) = 0;
    virtual void deleteTexture() = 0;
    
};

#endif /* TextureRender_h */
