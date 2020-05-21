#ifndef SupernovaWeb_h
#define SupernovaWeb_h

#include <emscripten/html5.h>
#include "system/System.h"

class SupernovaWeb: public Supernova::System{

private:

    static int syncWaitTime;
    static bool enabledIDB;

    static int screenWidth;
    static int screenHeight;

    static EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData);
    static EM_BOOL mouse_callback(int eventType, const EmscriptenMouseEvent *e, void *userData);
    static EM_BOOL wheel_callback(int eventType, const EmscriptenWheelEvent *e, void *userData);
    static EM_BOOL canvas_resize(int eventType, const void *reserved, void *userData);

    static void renderLoop();

    static void unicode_to_utf8(char *b, unsigned long c);
    static size_t utf8len(const char *s);
    static int supernova_mouse_button(int button);
    static int supernova_legacy_input(int code);
    static int supernova_input(const char code[32]);

public:

    SupernovaWeb();

    static void setEnabledIDB(bool enabledIDB);

    static int init(int width, int height);
    static void changeCanvasSize(int width, int height);

    virtual int getScreenWidth();
    virtual int getScreenHeight();

    virtual bool isFullscreen();
    virtual void requestFullscreen();
    virtual void exitFullscreen();

    virtual std::string getUserDataPath();

    virtual bool syncFileSystem();
    
};


#endif /* SupernovaWeb_h */