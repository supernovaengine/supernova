
#ifndef TextureRect_h
#define TextureRect_h

class TextureRect{
private:
    
    float x, y, width, height;
    
public:
    
    TextureRect();
    TextureRect(float x, float y, float width, float height);
    TextureRect(const TextureRect& t);
    
    TextureRect& operator = (const TextureRect& t);
    
    float getX();
    float getY();
    float getWidth();
    float getHeight();
    
    void setRect(float x, float y, float width, float height);
};

#endif /* TextureRect_h */
