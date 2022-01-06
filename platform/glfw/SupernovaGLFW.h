//
// (c) 2022 Eduardo Doria.
//

#ifndef SupernovaGLFW_h
#define SupernovaGLFW_h

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "System.h"

class SupernovaGLFW: public Supernova::System{

private:

    static int windowPosX;
    static int windowPosY;
    static int windowWidth;
    static int windowHeight;  

    static int screenWidth;
    static int screenHeight;

    static GLFWwindow* window;
    static GLFWmonitor* monitor;

public:

    SupernovaGLFW();

    static int init(int argc, char **argv);

    virtual int getScreenWidth();
    virtual int getScreenHeight();

    virtual sg_context_desc getSokolContext();

    virtual bool isFullscreen();
    virtual void requestFullscreen();
    virtual void exitFullscreen();

    virtual std::string getAssetPath();
    virtual std::string getLuaPath();
    
};


#endif /* SupernovaGLFW_h */