package com.deslon.supernova;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

import android.app.Activity;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView.Renderer;

public class RendererWrapper implements Renderer{
	
	private final MainActivity mainActivity;
	private final AssetManager assetManager;
	
	public RendererWrapper(MainActivity mainActivity, AssetManager assetManager) {
		this.mainActivity = mainActivity;
		this.assetManager = assetManager;
	}
	
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		JNIWrapper.init_native(mainActivity, assetManager);
		JNIWrapper.system_start();
		JNIWrapper.system_surface_created();
	}

	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height) {
		this.mainActivity.screenWidth = width;
		this.mainActivity.screenHeight = height;
		JNIWrapper.system_surface_changed();
	}

	@Override
	public void onDrawFrame(GL10 gl) {
		JNIWrapper.system_draw();
	}
}
