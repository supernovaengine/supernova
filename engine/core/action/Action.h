#ifndef Action_h
#define Action_h

typedef struct lua_State lua_State;

namespace Supernova{

    class Object;

    class Action{

        friend class Object;

    private:

        void (*onStartFunc)(Object*);
        int onStartLuaFunc;

        void (*onRunFunc)(Object*);
        int onRunLuaFunc;

        void (*onPauseFunc)(Object*);
        int onPauseLuaFunc;

        void (*onStopFunc)(Object*);
        int onStopLuaFunc;

        void (*onFinishFunc)(Object*);
        int onFinishLuaFunc;

        void (*onStepFunc)(Object*);
        int onStepLuaFunc;
        
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

        void onStart(void (*onStartFunc)(Object*));
        int onStart(lua_State *L);

        void onRun(void (*onRunFunc)(Object*));
        int onRun(lua_State *L);

        void onPause(void (*onPauseFunc)(Object*));
        int onPause(lua_State *L);

        void onStop(void (*onStopFunc)(Object*));
        int onStop(lua_State *L);

        void onFinish(void (*onFinishFunc)(Object*));
        int onFinish(lua_State *L);

        void onStep(void (*onStepFunc)(Object*));
        int onStep(lua_State *L);

        Object* getObject();

        bool isRunning();

        virtual bool run();
        virtual bool pause();
        virtual bool stop();
        
        virtual bool step();
    };
}

#endif /* Action_h */
