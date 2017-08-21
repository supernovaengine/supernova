#ifndef Material_h
#define Material_h


#include "Texture.h"
#include <string>
#include "math/Vector4.h"
#include "math/Rect.h"
#include <vector>

namespace Supernova {

    class Material {
    private:
        
        Texture* texture;
        Vector4 color;
        Rect* textureRect; //normalizaded
        
        bool textureOwned;
        
    public:
        Material();
        Material(const Material& s);
        virtual ~Material();
        
        bool transparent; //TODO: not public
        
        Material& operator = (const Material& s);
        
        void setTexture(Texture* texture);
        void setTexturePath(std::string texture_path);
        void setColor(Vector4 color);
        void setTextureCube(std::string front, std::string back, std::string left, std::string right, std::string up, std::string down);
        void setTextureRect(float x, float y, float width, float height);
        
        std::string getTexturePath();
        Texture* getTexture();
        Vector4* getColor();
        Rect* getTextureRect();
        
    };
        
}

#endif /* Material_h */
