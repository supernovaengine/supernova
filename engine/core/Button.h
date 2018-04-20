#ifndef Button_h
#define Button_h

#include "GUIImage.h"

namespace Supernova {

    class Button: public GUIImage {
    private:
        bool ownedTextureNormal;
        bool ownedTexturePressed;
        bool ownedTextureDisabled;
        
        Texture* textureNormal;
        Texture* texturePressed;
        Texture* textureDisabled;
        
    public:
        Button();
        virtual ~Button();
        
        void setTexturePressed();
        void setTextureDisabled();
        
        void setTextureNormal(Texture* texture);
        void setTextureNormal(std::string texturepath);
        std::string getTextureNormal();
        
        void setTexturePressed(Texture* texture);
        void setTexturePressed(std::string texturepath);
        std::string getTexturePressed();
        
        void setTextureDisabled(Texture* texture);
        void setTextureDisabled(std::string texturepath);
        std::string getTextureDisabled();

        virtual void call_onPress();
        virtual void call_onUp();
        
        virtual bool load();
    };
    
}

#endif /* Button_h */
