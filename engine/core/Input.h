#ifndef Input_h
#define Input_h

#define S_KEY_SPACE               32
#define S_KEY_APOSTROPHE          39  /* ' */
#define S_KEY_COMMA               44  /* , */
#define S_KEY_MINUS               45  /* - */
#define S_KEY_PERIOD              46  /* . */
#define S_KEY_SLASH               47  /* / */
#define S_KEY_0                   48
#define S_KEY_1                   49
#define S_KEY_2                   50
#define S_KEY_3                   51
#define S_KEY_4                   52
#define S_KEY_5                   53
#define S_KEY_6                   54
#define S_KEY_7                   55
#define S_KEY_8                   56
#define S_KEY_9                   57
#define S_KEY_SEMICOLON           59 /* ; */
#define S_KEY_EQUAL               61  /* = */
#define S_KEY_A                   65
#define S_KEY_B                   66
#define S_KEY_C                   67
#define S_KEY_D                   68
#define S_KEY_E                   69
#define S_KEY_F                   70
#define S_KEY_G                   71
#define S_KEY_H                   72
#define S_KEY_I                   73
#define S_KEY_J                   74
#define S_KEY_K                   75
#define S_KEY_L                   76
#define S_KEY_M                   77
#define S_KEY_N                   78
#define S_KEY_O                   79
#define S_KEY_P                   80
#define S_KEY_Q                   81
#define S_KEY_R                   82
#define S_KEY_S                   83
#define S_KEY_T                   84
#define S_KEY_U                   85
#define S_KEY_V                   86
#define S_KEY_W                   87
#define S_KEY_X                   88
#define S_KEY_Y                   89
#define S_KEY_Z                   90
#define S_KEY_LEFT_BRACKET        91 /* [ */
#define S_KEY_BACKSLASH           92  /* \ */
#define S_KEY_RIGHT_BRACKET       93  /* ] */
#define S_KEY_GRAVE_ACCENT        96 /* ` */

#define S_KEY_ESCAPE              256
#define S_KEY_ENTER               257
#define S_KEY_TAB                 258
#define S_KEY_BACKSPACE           259
#define S_KEY_INSERT              260
#define S_KEY_DELETE              261
#define S_KEY_RIGHT               262
#define S_KEY_LEFT                263
#define S_KEY_DOWN                264
#define S_KEY_UP                  265
#define S_KEY_PAGE_UP             266
#define S_KEY_PAGE_DOWN           267
#define S_KEY_HOME                268
#define S_KEY_END                 269
#define S_KEY_CAPS_LOCK           280
#define S_KEY_SCROLL_LOCK         281
#define S_KEY_NUM_LOCK            282
#define S_KEY_PRINT_SCREEN        283
#define S_KEY_PAUSE               284
#define S_KEY_F1                  290
#define S_KEY_F2                  291
#define S_KEY_F3                  292
#define S_KEY_F4                  293
#define S_KEY_F5                  294
#define S_KEY_F6                  295
#define S_KEY_F7                  296
#define S_KEY_F8                  297
#define S_KEY_F9                  298
#define S_KEY_F10                 299
#define S_KEY_F11                 300
#define S_KEY_F12                 301
#define S_KEY_KP_0                320
#define S_KEY_KP_1                321
#define S_KEY_KP_2                322
#define S_KEY_KP_3                323
#define S_KEY_KP_4                324
#define S_KEY_KP_5                325
#define S_KEY_KP_6                326
#define S_KEY_KP_7                327
#define S_KEY_KP_8                328
#define S_KEY_KP_9                329
#define S_KEY_KP_DECIMAL          330
#define S_KEY_KP_DIVIDE           331
#define S_KEY_KP_MULTIPLY         332
#define S_KEY_KP_SUBTRACT         333
#define S_KEY_KP_ADD              334
#define S_KEY_KP_ENTER            335
#define S_KEY_KP_EQUAL            336
#define S_KEY_LEFT_SHIFT          340
#define S_KEY_LEFT_CONTROL        341
#define S_KEY_LEFT_ALT            342
#define S_KEY_LEFT_SUPER          343
#define S_KEY_RIGHT_SHIFT         344
#define S_KEY_RIGHT_CONTROL       345
#define S_KEY_RIGHT_ALT           346
#define S_KEY_RIGHT_SUPER         347
#define S_KEY_MENU                348

//Mouse
#define S_MOUSE_BUTTON_1    1
#define S_MOUSE_BUTTON_2    2
#define S_MOUSE_BUTTON_3    3
#define S_MOUSE_BUTTON_4    4
#define S_MOUSE_BUTTON_5    5
#define S_MOUSE_BUTTON_6    6
#define S_MOUSE_BUTTON_7    7
#define S_MOUSE_BUTTON_8    8

#define S_MOUSE_BUTTON_LAST     S_MOUSE_BUTTON_8
#define S_MOUSE_BUTTON_LEFT     S_MOUSE_BUTTON_1
#define S_MOUSE_BUTTON_RIGHT    S_MOUSE_BUTTON_2
#define S_MOUSE_BUTTON_MIDDLE   S_MOUSE_BUTTON_3

#include <map>
#include "math/Vector2.h"

namespace Supernova {
    
    class Input {
        
        friend class Events;
        
    private:
        
        static std::map<int,bool> keyPressed;
        static std::map<int,bool> mousePressed;
        static bool touchStarted;
        
        static Vector2 mousePosition;
        static Vector2 touchPosition;
        
        static void addKeyPressed(int key);
        static void releaseKeyPressed(int key);
        
        static void addMousePressed(int key);
        static void releaseMousePressed(int key);
        static void setMousePosition(float x, float y);
        
        static void addTouchStarted();
        static void releaseTouchStarted();
        static void setTouchPosition(float x, float y);
        
    public:
        
        static bool isKeyPressed(int key);
        static bool isMousePressed(int key);
        static bool isTouch();
        
        static Vector2 getMousePosition();
        static Vector2 getTouchPosition();
    };
}

#endif /* Input_h */
