#ifndef Button_h
#define Button_h

#define S_BUTTON_NORMAL 10
#define S_BUTTON_PRESSED 11
#define S_BUTTON_DISABLED 12

#include "GUIImage.h"
#include "Text.h"

namespace Supernova {

    class Button: public GUIImage {
    private:
        bool ownedTextureNormal;
        bool ownedTexturePressed;
        bool ownedTextureDisabled;
        
        Texture* textureNormal;
        Texture* texturePressed;
        Texture* textureDisabled;

        bool down;
        int pointerDown;

        Text label;
        
    public:
        Button();
        virtual ~Button();
        
        void setLabelText(std::string text);
        void setLabelFont(std::string font);
        void setLabelSize(unsigned int size);
        void setLabelColor(Vector4 color);
        Text* getLabel();
        
        void setTextureNormal(Texture* texture);
        void setTextureNormal(std::string texturepath);
        std::string getTextureNormal();
        
        void setTexturePressed(Texture* texture);
        void setTexturePressed(std::string texturepath);
        std::string getTexturePressed();
        
        void setTextureDisabled(Texture* texture);
        void setTextureDisabled(std::string texturepath);
        std::string getTextureDisabled();

        virtual void engine_onDown(int pointer, float x, float y);
        virtual void engine_onUp(int pointer, float x, float y);

        bool isDown();
        
        virtual bool load();
    };
    
}

#endif /* Button_h */
