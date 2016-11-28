#ifndef engine_h
#define engine_h

class Engine {

public:

    static void onStart();
	static void onStart(int width, int height);
	static void onSurfaceCreated();
	static void onSurfaceChanged(int width, int height);
	static void onDrawFrame();

	static void onPause();
	static void onResume();

    static void onTouchPress(float x, float y);
    static void onTouchUp(float x, float y);
    static void onTouchDrag(float x, float y);

    static void onMousePress(int button, float x, float y);
    static void onMouseUp(int button, float x, float y);
    static void onMouseDrag(int button, float x, float y);
    static void onMouseMove(float x, float y);

    static void onKeyPress(int inputKey);
    static void onKeyUp(int inputKey);

    static void onTextInput(const char* text);

    static int getScreenWidth();
    static int getScreenHeight();

};

#endif /* engine_h */
