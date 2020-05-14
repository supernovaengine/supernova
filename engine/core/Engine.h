#ifndef engine_h
#define engine_h

#include "util/FunctionSubscribe.h"

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
        
        static int canvasWidth;
        static int canvasHeight;
        
        static int preferedCanvasWidth;
        static int preferedCanvasHeight;
        
        static Rect viewRect;
        
        static int renderAPI;
        static bool callMouseInTouchEvent;
        static bool callTouchInMouseEvent;
        static bool useDegrees;
        static Scaling scalingMode;
        static bool defaultNearestScaleTexture;
        static bool defaultResampleToPOTTexture;
        static bool fixedTimeSceneUpdate;
        static bool fixedTimePhysics;
        static bool fixedTimeAnimations;

        static unsigned long lastTime;
        static float updateTimeCount;
        
        static float deltatime;
        static float framerate;
        
        static float updateTime;
        
        static bool transformCoordPos(float& x, float& y);

    public:
        
        Engine();
        virtual ~Engine();
        
        //-----Supernova config-----
        static void setScene(Scene *mainScene);
        static Scene* getScene();
        
        static int getCanvasWidth();
        static int getCanvasHeight();
        static void setCanvasSize(int canvasWidth, int canvasHeight);
        
        static int getPreferedCanvasWidth();
        static int getPreferedCanvasHeight();

        static void calculateCanvas();
        
        static Rect* getViewRect();
        
        static void setRenderAPI(int renderAPI);
        static int getRenderAPI();
        
        static void setScalingMode(Scaling scalingMode);
        static int getScalingMode();
        
        static void setCallMouseInTouchEvent(bool callMouseInTouchEvent);
        static bool isCallMouseInTouchEvent();

        static void setCallTouchInMouseEvent(bool callTouchInMouseEvent);
        static bool isCallTouchInMouseEvent();
        
        static void setUseDegrees(bool useDegrees);
        static bool isUseDegrees();
        
        static void setDefaultNearestScaleTexture(bool defaultNearestScaleTexture);
        static bool isDefaultNearestScaleTexture();

        static void setDefaultResampleToPOTTexture(bool defaultResampleToPOTTexture);
        static bool isDefaultResampleToPOTTexture();

        static void setFixedTimeSceneUpdate(bool fixedTimeSceneUpdate);
        static bool isFixedTimeSceneUpdate();

        static void setUpdateTime(unsigned int updateTimeMS);
        static float getUpdateTime();

        static float getSceneUpdateTime();
        
        static int getPlatform();
        static float getFramerate();
        static float getDeltatime();
        
        //-----Supernova API functions-----
        static void systemStart();
        static void systemSurfaceCreated();
        static void systemSurfaceChanged();
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
        static FunctionSubscribe<void()> onCanvasLoaded;
        static FunctionSubscribe<void()> onCanvasChanged;
        static FunctionSubscribe<void()> onDraw;
        static FunctionSubscribe<void()> onUpdate;
        static FunctionSubscribe<void(int,float,float)> onTouchStart;
        static FunctionSubscribe<void(int,float,float)> onTouchEnd;
        static FunctionSubscribe<void(int,float,float)> onTouchDrag;
        static FunctionSubscribe<void(int,float,float)> onMouseDown;
        static FunctionSubscribe<void(int,float,float)> onMouseUp;
        static FunctionSubscribe<void(int,float,float)> onMouseDrag;
        static FunctionSubscribe<void(float,float)> onMouseMove;
        static FunctionSubscribe<void(int)> onKeyDown;
        static FunctionSubscribe<void(int)> onKeyUp;
        static FunctionSubscribe<void(std::string)> onTextInput;

    };
    
}

#endif /* engine_h */
