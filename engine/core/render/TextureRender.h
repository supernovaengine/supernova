
#ifndef TextureRender_h
#define TextureRender_h

class TextureRender {
    
public:
    
    inline virtual ~TextureRender(){}
    
    virtual void loadTexture(const char* relative_path) = 0;
    virtual void deleteTexture() = 0;
    
};

#endif /* TextureRender_h */
