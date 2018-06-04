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

typedef struct lua_State lua_State;

namespace Supernova {

    class Scene;
    class Rect;

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
        static int scalingMode;
        static bool nearestScaleTexture;
        static bool fixedTimePhysics;

        static unsigned long lastTime;
        static unsigned int updateTimeCount;
        
        static unsigned int deltatime;
        static float framerate;
        
        static unsigned int updateTime;
        
        static bool transformCoordPos(float& x, float& y);

        //-----Supernova user events-----
        static void (*onCanvasLoadedFunc)();
        static int onCanvasLoadedLuaFunc;

        static void (*onCanvasChangedFunc)();
        static int onCanvasChangedLuaFunc;

        static void (*onDrawFunc)();
        static int onDrawLuaFunc;

        static void (*onUpdateFunc)();
        static int onUpdateLuaFunc;

        static void (*onTouchStartFunc)(int, float, float);
        static int onTouchStartLuaFunc;

        static void (*onTouchEndFunc)(int, float, float);
        static int onTouchEndLuaFunc;

        static void (*onTouchDragFunc)(int, float, float);
        static int onTouchDragLuaFunc;

        static void (*onMouseDownFunc)(int, float, float);
        static int onMouseDownLuaFunc;

        static void (*onMouseUpFunc)(int, float, float);
        static int onMouseUpLuaFunc;

        static void (*onMouseDragFunc)(int, float, float);
        static int onMouseDragLuaFunc;

        static void (*onMouseMoveFunc)(float, float);
        static int onMouseMoveLuaFunc;

        static void (*onKeyDownFunc)(int);
        static int onKeyDownLuaFunc;

        static void (*onKeyUpFunc)(int);
        static int onKeyUpLuaFunc;

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

        static void setFixedTimePhysics(bool fixedTimePhysics);
        static bool isFixedTimePhysics();
        
        static void setUpdateTime(unsigned int updateTime);
        static unsigned int getUpdateTime();
        
        static int getPlatform();
        static float getFramerate();
        static float getDeltatime();
        
        //-----Supernova API events-----
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
        static void onCanvasLoaded(void (*onCanvasLoadedFunc)());
        static int onCanvasLoaded(lua_State *L);
        static void call_onCanvasLoaded();

        static void onCanvasChanged(void (*onCanvasChangedFunc)());
        static int onCanvasChanged(lua_State *L);
        static void call_onCanvasChanged();

        static void onDraw(void (*onDrawFunc)());
        static int onDraw(lua_State *L);
        static void call_onDraw();

        static void onUpdate(void (*onUpdateFunc)());
        static int onUpdate(lua_State *L);
        static void call_onUpdate();

        static void onTouchStart(void (*onTouchStartFunc)(int, float, float));
        static int onTouchStart(lua_State *L);
        static void call_onTouchStart(int pointer, float x, float y);

        static void onTouchEnd(void (*onTouchEndFunc)(int, float, float));
        static int onTouchEnd(lua_State *L);
        static void call_onTouchEnd(int pointer, float x, float y);

        static void onTouchDrag(void (*onTouchDragFunc)(int, float, float));
        static int onTouchDrag(lua_State *L);
        static void call_onTouchDrag(int pointer, float x, float y);

        static void onMouseDown(void (*onMouseDownFunc)(int, float, float));
        static int onMouseDown(lua_State *L);
        static void call_onMouseDown(int button, float x, float y);

        static void onMouseUp(void (*onMouseUpFunc)(int, float, float));
        static int onMouseUp(lua_State *L);
        static void call_onMouseUp(int button, float x, float y);

        static void onMouseDrag(void (*onMouseDragFunc)(int, float, float));
        static int onMouseDrag(lua_State *L);
        static void call_onMouseDrag(int button, float x, float y);

        static void onMouseMove(void (*onMouseMoveFunc)(float, float));
        static int onMouseMove(lua_State *L);
        static void call_onMouseMove(float x, float y);

        static void onKeyDown(void (*onKeyDownFunc)(int));
        static int onKeyDown(lua_State *L);
        static void call_onKeyDown(int key);

        static void onKeyUp(void (*onKeyUpFunc)(int));
        static int onKeyUp(lua_State *L);
        static void call_onKeyUp(int key);

    };
    
}

#endif /* engine_h */
