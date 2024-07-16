//
// (c) 2024 Eduardo Doria.
//

#include "SupernovaGLFW.h"

#include "Engine.h"

int SupernovaGLFW::windowPosX;
int SupernovaGLFW::windowPosY;
int SupernovaGLFW::windowWidth;
int SupernovaGLFW::windowHeight;  

int SupernovaGLFW::screenWidth;
int SupernovaGLFW::screenHeight;

double SupernovaGLFW::mousePosX;
double SupernovaGLFW::mousePosY;

int SupernovaGLFW::sampleCount;

GLFWwindow* SupernovaGLFW::window;
GLFWmonitor* SupernovaGLFW::monitor;


SupernovaGLFW::SupernovaGLFW(){

}

int SupernovaGLFW::init(int argc, char **argv){
    windowWidth = DEFAULT_WINDOW_WIDTH;
    windowHeight = DEFAULT_WINDOW_HEIGHT;

    sampleCount = 1;

    Supernova::Engine::systemInit(argc, argv);

    /* create window and GL context via GLFW */
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, (sampleCount == 1) ? 0 : sampleCount);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(windowWidth, windowHeight, "Supernova", 0, 0);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    monitor = glfwGetPrimaryMonitor();

    glfwSetMouseButtonCallback(window, [](GLFWwindow*, int btn, int action, int mods) {
        if (action==GLFW_PRESS){
            Supernova::Engine::systemMouseDown(btn, float(mousePosX), float(mousePosY), mods);
        }else if (action==GLFW_RELEASE){
            Supernova::Engine::systemMouseUp(btn, float(mousePosX), float(mousePosY), mods);
        }
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow*, double pos_x, double pos_y) {
        float xscale, yscale;
        glfwGetWindowContentScale(window, &xscale, &yscale);

        mousePosX = pos_x * xscale;
        mousePosY = pos_y * yscale;
        Supernova::Engine::systemMouseMove(float(pos_x), float(pos_y), 0);
    });
    glfwSetScrollCallback(window, [](GLFWwindow*, double pos_x, double pos_y){
        Supernova::Engine::systemMouseScroll((float)pos_x, (float)pos_y, 0);
    });
    glfwSetKeyCallback(window, [](GLFWwindow*, int key, int /*scancode*/, int action, int mods){
        if (action==GLFW_PRESS){
            if (key == GLFW_KEY_TAB)
                Supernova::Engine::systemCharInput('\t');
            if (key == GLFW_KEY_BACKSPACE)
                Supernova::Engine::systemCharInput('\b');
            if (key == GLFW_KEY_ENTER)
                Supernova::Engine::systemCharInput('\r');
            if (key == GLFW_KEY_ESCAPE)
                Supernova::Engine::systemCharInput('\e');
            Supernova::Engine::systemKeyDown(key, false, mods);
        }else if (action==GLFW_REPEAT){
            Supernova::Engine::systemKeyDown(key, true, mods);
        }else if (action==GLFW_RELEASE){
            Supernova::Engine::systemKeyUp(key, false, mods);
        }
    });
    glfwSetCharCallback(window, [](GLFWwindow*, unsigned int codepoint){
        Supernova::Engine::systemCharInput(codepoint);
    });

    int cur_width, cur_height;
    glfwGetFramebufferSize(window, &cur_width, &cur_height);

    SupernovaGLFW::screenWidth = cur_width;
    SupernovaGLFW::screenHeight = cur_height;

    Supernova::Engine::systemViewLoaded();
    Supernova::Engine::systemViewChanged();

    /* draw loop */
    while (!glfwWindowShouldClose(window)) {
        int cur_width, cur_height;
        glfwGetFramebufferSize(window, &cur_width, &cur_height);

        if (cur_width != SupernovaGLFW::screenWidth || cur_height != SupernovaGLFW::screenHeight){
            SupernovaGLFW::screenWidth = cur_width;
            SupernovaGLFW::screenHeight = cur_height;
            Supernova::Engine::systemViewChanged();
        }

        Supernova::Engine::systemDraw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    Supernova::Engine::systemViewDestroyed();
    Supernova::Engine::systemShutdown();
    glfwTerminate();
    return 0;
}

int SupernovaGLFW::getScreenWidth(){
    return SupernovaGLFW::screenWidth;
}

int SupernovaGLFW::getScreenHeight(){
    return SupernovaGLFW::screenHeight;
}

int SupernovaGLFW::getSampleCount(){
    return SupernovaGLFW::sampleCount;
}

bool SupernovaGLFW::isFullscreen(){
    return glfwGetWindowMonitor(window) != nullptr;
}

void SupernovaGLFW::requestFullscreen(){
    if (isFullscreen())
        return;

    // backup window position and window size
    glfwGetWindowPos(window, &windowPosX, &windowPosY);
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
        
    // get resolution of monitor
    const GLFWvidmode * mode = glfwGetVideoMode(monitor);

    // switch to full screen
    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 0 );
}

void SupernovaGLFW::exitFullscreen(){
    if (!isFullscreen())
        return;

    // restore last window size and position
    glfwSetWindowMonitor(window, nullptr,  windowPosX, windowPosY, windowWidth, windowHeight, 0);
}

void SupernovaGLFW::setMouseCursor(Supernova::CursorType type){
    GLFWcursor* cursor = NULL;

    if (type == Supernova::CursorType::ARROW){
        cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    }else if (type == Supernova::CursorType::IBEAM){
        cursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    }else if (type == Supernova::CursorType::CROSSHAIR){
        cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    }else if (type == Supernova::CursorType::POINTING_HAND){
        #ifdef GLFW_POINTING_HAND_CURSOR
        cursor = glfwCreateStandardCursor(GLFW_POINTING_HAND_CURSOR);
        #else
        cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
        #endif
    }else if (type == Supernova::CursorType::RESIZE_EW){
        #ifdef GLFW_RESIZE_EW_CURSOR
        cursor = glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR);
        #else
        cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        #endif
    }else if (type == Supernova::CursorType::RESIZE_NS){
        #ifdef GLFW_RESIZE_NS_CURSOR
        cursor = glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR);
        #else
        cursor = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
        #endif
    }else if (type == Supernova::CursorType::RESIZE_NWSE){
        #ifdef GLFW_RESIZE_NWSE_CURSOR
        cursor = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
        #else
        cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        #endif
    }else if (type == Supernova::CursorType::RESIZE_NESW){
        #ifdef GLFW_RESIZE_NESW_CURSOR
        cursor = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
        #else
        cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        #endif
    }else if (type == Supernova::CursorType::RESIZE_ALL){
        #ifdef GLFW_RESIZE_ALL_CURSOR
        cursor = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
        #else
        cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        #endif
    }else if (type == Supernova::CursorType::NOT_ALLOWED){
        #ifdef GLFW_NOT_ALLOWED_CURSOR
        cursor = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
        #else
        cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        #endif
    }

    if (cursor) {
        glfwSetCursor(window, cursor);
    } else {
        // Handle error: cursor creation failed
    }
}

void SupernovaGLFW::setShowCursor(bool showCursor){
    if (showCursor){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }else{
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
}

std::string SupernovaGLFW::getAssetPath(){
    return "assets";
}

std::string SupernovaGLFW::getUserDataPath(){
    return ".";
}

std::string SupernovaGLFW::getLuaPath(){
    return "lua";
}