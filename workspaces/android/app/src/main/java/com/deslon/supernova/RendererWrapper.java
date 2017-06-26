package com.deslon.supernova;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

import android.app.Activity;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView.Renderer;

public class RendererWrapper implements Renderer{
	
	private final Activity mainActivity;
	private final AssetManager assetManager;
	private final int displayWidth, displayHeight;
	
	public RendererWrapper(Activity mainActivity, AssetManager assetManager, int displayWidth, int displayHeight) {
		this.mainActivity = mainActivity;
		this.assetManager = assetManager;
		this.displayWidth = displayWidth;
		this.displayHeight = displayHeight;
	}
	
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		JNIWrapper.init_native(mainActivity, assetManager);
		JNIWrapper.on_start(displayWidth, displayHeight);
		JNIWrapper.on_surface_created();
	}

	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height) {
		JNIWrapper.on_surface_changed(width, height);
	}

	@Override
	public void onDrawFrame(GL10 gl) {
		JNIWrapper.on_draw();
	}
}
