package com.deslon.supernova;

import android.app.Activity;
import android.content.res.AssetManager;

public class JNIWrapper {
	
    static {
        System.loadLibrary("supernova-android");
    }

	public static native void init_native(Activity mainActivity, AssetManager assetManager);
    
    public static native void system_start();
    
	public static native void system_surface_created();

	public static native void system_surface_changed();

	public static native void system_draw();
	
	public static native void system_pause();

	public static native void system_resume();
	
	public static native void system_touch_start(int pointer, float x, float y);
	
	public static native void system_touch_end(int pointer, float x, float y);
	
	public static native void system_touch_drag(int pointer, float x, float y);
	
	public static native void system_key_down(int key);
	
	public static native void system_key_up(int key);
	
	public static native void system_text_input(String text);

}
