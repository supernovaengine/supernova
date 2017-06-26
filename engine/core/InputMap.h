#ifndef InputMap_h
#define InputMap_h

#include <map>
#include "math/Vector2.h"

namespace Supernova {
    
    class InputMap {
        
        friend class Events;
        
    private:
        static std::map<int,bool> keyPressed;
        static std::map<int,bool> mousePressed;
        static bool touchPressed;
        
        static Vector2 mousePosition;
        static Vector2 touchPosition;
        
        
        static void addKeyPressed(int key);
        static void releaseKeyPressed(int key);
        
        static void addMousePressed(int key);
        static void releaseMousePressed(int key);
        static void setMousePosition(float x, float y);
        
        static void addTouchPressed();
        static void releaseTouchPressed();
        static void setTouchPosition(float x, float y);
        
    public:
        
        static bool isKeyPressed(int key);
        static bool isMousePressed(int key);
        static bool isTouch();
        
        static Vector2 getMousePosition();
        static Vector2 getTouchPosition();
    };
}

#endif /* InputMap_h */
