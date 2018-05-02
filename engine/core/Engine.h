#ifndef engine_h
#define engine_h


#define S_GLES2 1

#define S_SCALING_FITWIDTH 1
#define S_SCALING_FITHEIGHT 2
#define S_SCALING_LETTERBOX 3
#define S_SCALING_CROP 4
#define S_SCALING_STRETCH 5

#define S_PLATFORM_ANDROID 1
#define S_PLATFORM_IOS 2
#define S_PLATFORM_WEB 3

namespace Supernova {

    class Scene;
    class Rect;

    class Engine {
        
    private:
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
        static int scalingMode;
        static bool nearestScaleTexture;

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
        
        static void setScalingMode(int scalingMode);
        static int getScalingMode();
        
        static void setMouseAsTouch(bool mouseAsTouch);
        static bool isMouseAsTouch();
        
        static void setUseDegrees(bool useDegrees);
        static bool isUseDegrees();
        
        static void setNearestScaleTexture(bool nearestScaleTexture);
        static bool isNearestScaleTexture();
        
        static void setUpdateTime(unsigned int updateTime);
        static unsigned int getUpdateTime();
        
        static int getPlatform();
        static float getFramerate();
        static float getDeltatime();
        
        //-----Supernova API events-----
        static void onStart();
        static void onStart(int width, int height);
        static void onSurfaceCreated();
        static void onSurfaceChanged(int width, int height);
        static void onDraw();

        static void onPause();
        static void onResume();

        static void onTouchStart(float x, float y);
        static void onTouchEnd(float x, float y);
        static void onTouchDrag(float x, float y);

        static void onMouseDown(int button, float x, float y);
        static void onMouseUp(int button, float x, float y);
        static void onMouseDrag(int button, float x, float y);
        static void onMouseMove(float x, float y);

        static void onKeyDown(int inputKey);
        static void onKeyUp(int inputKey);

        static void onTextInput(const char* text);

    };
    
}

#endif /* engine_h */
