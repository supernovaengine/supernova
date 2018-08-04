#ifndef engine_h
#define engine_h

#include "util/FunctionCallback.h"

//
// (c) 2018 Eduardo Doria.
//

#define S_GLES2 1

#define S_PLATFORM_ANDROID 1
#define S_PLATFORM_IOS 2
#define S_PLATFORM_WEB 3

namespace Supernova {

    class Scene;
    class Rect;

    enum Scaling{
        FITWIDTH,
        FITHEIGHT,
        LETTERBOX,
        CROP,
        STRETCH
    };

    class Engine {
        
    private:
        //-----Supernova config-----
        static Scene *mainScene;
        
        static int screenWidth;
        static int screenHeight;
        
        static int canvasWidth;
        static int canvasHeight;
        
        static int preferedCanvasWidth;
        static int preferedCanvasHeight;
        
        static Rect viewRect;
        
        static int renderAPI;
        static bool mouseAsTouch;
        static bool useDegrees;
        static Scaling scalingMode;
        static bool defaultNearestScaleTexture;
        static bool defaultResampleToPOTTexture;
        static bool fixedTimePhysics;

        static unsigned long lastTime;
        static unsigned int updateTimeCount;
        
        static unsigned int deltatime;
        static float framerate;
        
        static unsigned int updateTime;
        
        static bool transformCoordPos(float& x, float& y);

    public:
        
        Engine();
        virtual ~Engine();
        
        //-----Supernova config-----
        static void setScene(Scene *mainScene);
        static Scene* getScene();
        
        static int getScreenWidth();
        static int getScreenHeight();
        static void setScreenSize(int screenWidth, int screenHeight);
        
        static int getCanvasWidth();
        static int getCanvasHeight();
        static void setCanvasSize(int canvasWidth, int canvasHeight);
        
        static int getPreferedCanvasWidth();
        static int getPreferedCanvasHeight();
        static void setPreferedCanvasSize(int preferedCanvasWidth, int preferedCanvasHeight);
        
        static Rect* getViewRect();
        
        static void setRenderAPI(int renderAPI);
        static int getRenderAPI();
        
        static void setScalingMode(Scaling scalingMode);
        static int getScalingMode();
        
        static void setMouseAsTouch(bool mouseAsTouch);
        static bool isMouseAsTouch();
        
        static void setUseDegrees(bool useDegrees);
        static bool isUseDegrees();
        
        static void setDefaultNearestScaleTexture(bool defaultNearestScaleTexture);
        static bool isDefaultNearestScaleTexture();

        static void setDefaultResampleToPOTTexture(bool defaultResampleToPOTTexture);
        static bool isDefaultResampleToPOTTexture();

        static void setFixedTimePhysics(bool fixedTimePhysics);
        static bool isFixedTimePhysics();
        
        static void setUpdateTime(unsigned int updateTime);
        static unsigned int getUpdateTime();
        
        static int getPlatform();
        static float getFramerate();
        static float getDeltatime();
        
        //-----Supernova API functions-----
        static void systemStart();
        static void systemStart(int width, int height);
        static void systemSurfaceCreated();
        static void systemSurfaceChanged(int width, int height);
        static void systemDraw();

        static void systemPause();
        static void systemResume();

        static void systemTouchStart(int pointer, float x, float y);
        static void systemTouchEnd(int pointer, float x, float y);
        static void systemTouchDrag(int pointer, float x, float y);

        static void systemMouseDown(int button, float x, float y);
        static void systemMouseUp(int button, float x, float y);
        static void systemMouseDrag(int button, float x, float y);
        static void systemMouseMove(float x, float y);

        static void systemKeyDown(int inputKey);
        static void systemKeyUp(int inputKey);

        static void systemTextInput(const char* text);

        //-----Supernova user events-----
        static FunctionCallback<void()> onCanvasLoaded;
        static FunctionCallback<void()> onCanvasChanged;
        static FunctionCallback<void()> onDraw;
        static FunctionCallback<void()> onUpdate;
        static FunctionCallback<void(int,float,float)> onTouchStart;
        static FunctionCallback<void(int,float,float)> onTouchEnd;
        static FunctionCallback<void(int,float,float)> onTouchDrag;
        static FunctionCallback<void(int,float,float)> onMouseDown;
        static FunctionCallback<void(int,float,float)> onMouseUp;
        static FunctionCallback<void(int,float,float)> onMouseDrag;
        static FunctionCallback<void(float,float)> onMouseMove;
        static FunctionCallback<void(int)> onKeyDown;
        static FunctionCallback<void(int)> onKeyUp;
        static FunctionCallback<void(std::string)> onTextInput;

    };
    
}

#endif /* engine_h */
