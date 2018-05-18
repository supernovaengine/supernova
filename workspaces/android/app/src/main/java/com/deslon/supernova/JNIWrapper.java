package com.deslon.supernova;

import android.app.Activity;
import android.content.res.AssetManager;

public class JNIWrapper {
	
    static {
        System.loadLibrary("supernova-android");
    }
    
    public static native void on_start(int width, int height);
    
	public static native void on_surface_created();

	public static native void on_surface_changed(int width, int height);

	public static native void on_draw();
	
	public static native void on_pause();

	public static native void on_resume();
	
	public static native void init_native(Activity mainActivity, AssetManager assetManager);
	
	public static native void on_touch_start(float x, float y);
	
	public static native void on_touch_end(float x, float y);
	
	public static native void on_touch_drag(float x, float y);
	
	public static native void on_key_down(int key);
	
	public static native void on_key_up(int key);
	
	public static native void on_text_input(String text);

}
