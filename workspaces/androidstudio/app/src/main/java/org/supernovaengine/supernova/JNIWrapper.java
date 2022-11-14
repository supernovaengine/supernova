package org.supernovaengine.supernova;

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
	public static native void system_shutdown();

	public static native void system_draw();
	public static native void system_pause();
	public static native void system_resume();
	
	public static native void system_touch_start(int pointer, float x, float y);
	public static native void system_touch_end(int pointer, float x, float y);
	public static native void system_touch_move(int pointer, float x, float y);
	public static native void system_touch_cancel();
	
	public static native void system_key_down(int key, boolean repeat, int mods);
	public static native void system_key_up(int key, boolean repeat, int mods);
	
	public static native void system_char_input(int codepoint);

}
