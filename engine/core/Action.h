#ifndef Action_h
#define Action_h

typedef struct lua_State lua_State;

namespace Supernova{

    class Object;

    class Action{

        friend class Object;

    private:

        void (*onStartFunc)();
        int onStartLuaFunc;

        void (*onRunFunc)();
        int onRunLuaFunc;

        void (*onPauseFunc)();
        int onPauseLuaFunc;

        void (*onStopFunc)();
        int onStopLuaFunc;

        void (*onFinishFunc)();
        int onFinishLuaFunc;

        void (*onStepFunc)();
        int onStepLuaFunc;

        void luaCallback(int nargs, int nresults, int msgh);
        
    protected:
        
        unsigned long timecount;
        unsigned int steptime;
        
        bool running;
        Object* object;

        void call_onStart();
        void call_onRun();
        void call_onPause();
        void call_onStop();
        void call_onFinish();
        void call_onStep();
        
    public:
        Action();
        virtual ~Action();
        
        bool isRunning();

        void onStart(void (*onStartFunc)());
        int onStart(lua_State *L);

        void onRun(void (*onRunFunc)());
        int onRun(lua_State *L);

        void onPause(void (*onPauseFunc)());
        int onPause(lua_State *L);

        void onStop(void (*onStopFunc)());
        int onStop(lua_State *L);

        void onFinish(void (*onFinishFunc)());
        int onFinish(lua_State *L);

        void onStep(void (*onFinishFunc)());
        int onStep(lua_State *L);

        virtual bool run();
        virtual bool pause();
        virtual bool stop();
        
        virtual bool step();
    };
}

#endif /* Action_h */
