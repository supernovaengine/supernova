package com.deslon.supernova;

import java.io.IOException;

import android.annotation.SuppressLint;
import android.app.ActionBar;
import android.app.Activity;
import android.app.ActivityManager;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.ConfigurationInfo;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.View.OnKeyListener;
import android.view.Window;
import android.view.WindowManager;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.FrameLayout;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;


@SuppressLint("ClickableViewAccessibility")
public class MainActivity extends Activity {
    
	private GLSurfaceView glSurfaceView;
	private boolean rendererSet;

	private FrameLayout layout;
	private TextInput edittext;
    
    @Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		requestWindowFeature(Window.FEATURE_NO_TITLE);
	    getWindow().setFlags(
	            WindowManager.LayoutParams.FLAG_FULLSCREEN,
	            WindowManager.LayoutParams.FLAG_FULLSCREEN);

		ActivityManager activityManager 
			= (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
		ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();

		final boolean supportsEs2 = 
			configurationInfo.reqGlEsVersion >= 0x20000 || isProbablyEmulator();

		if (supportsEs2) {
			glSurfaceView = new GLSurfaceView(this);

			//if (isProbablyEmulator())
			glSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 24, 8);

			DisplayMetrics dm = new DisplayMetrics();
			getWindowManager().getDefaultDisplay().getMetrics(dm);
			final RendererWrapper rendererWrapper = new RendererWrapper(MainActivity.this, this.getAssets(), dm.widthPixels, dm.heightPixels);
			
			glSurfaceView.setEGLContextClientVersion(2);
			glSurfaceView.setPreserveEGLContextOnPause(true);
			glSurfaceView.setRenderer(rendererWrapper);
			glSurfaceView.setFocusableInTouchMode(true);
			rendererSet = true;
			//setContentView(glSurfaceView);

			layout = new FrameLayout(this);
			layout.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
			setContentView(layout);

			edittext = new TextInput(this);
			edittext.setLayoutParams(new ViewGroup.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT));
			edittext.setImeOptions(EditorInfo.IME_FLAG_NO_EXTRACT_UI);
			layout.addView(edittext);
			//addContentView(edittext, new ViewGroup.LayoutParams(LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));

			layout.addView(glSurfaceView);
			edittext.setView(glSurfaceView);
			
			glSurfaceView.setOnKeyListener(new KeyListener(glSurfaceView));
			
			glSurfaceView.setOnTouchListener(new OnTouchListener() {
				@Override
				public boolean onTouch(View v, MotionEvent event) {
	                if (event != null) {           
	                    final float normalizedX = (event.getX() / (float) v.getWidth()) * 2 - 1;
	                    final float normalizedY = (event.getY() / (float) v.getHeight()) * 2 - 1;
	                    
	                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
	                        glSurfaceView.queueEvent(new Runnable() {
	                            @Override
	                            public void run() {
	                            	JNIWrapper.on_touch_press(normalizedX, normalizedY);
	                            }
	                        });
	                    } else if (event.getAction() == MotionEvent.ACTION_UP) {
	                    	glSurfaceView.queueEvent(new Runnable() {
	                    		@Override
		                        public void run() {
	                    			JNIWrapper.on_touch_up(normalizedX, normalizedY);
		                        }
	                    	});
	                    } else if (event.getAction() == MotionEvent.ACTION_MOVE) {
	                        glSurfaceView.queueEvent(new Runnable() {
	                            @Override
	                            public void run() {
	                            	JNIWrapper.on_touch_drag(normalizedX, normalizedY);
	                            }
	                        });
	                    }                    

	                    return true;                    
	                } else {
	                    return false;
	                }
				}
	        });
			
		} else {
			// Should never be seen in production, since the manifest filters
			// unsupported devices.
			Toast.makeText(this, "This device does not support OpenGL ES 2.0.",
					Toast.LENGTH_LONG).show();
			return;
		}
	}
    
	private boolean isProbablyEmulator() {
		return Build.VERSION.SDK_INT >= Build.VERSION_CODES.ICE_CREAM_SANDWICH_MR1
				&& (Build.FINGERPRINT.startsWith("generic")
						|| Build.FINGERPRINT.startsWith("unknown")
						|| Build.MODEL.contains("google_sdk")
						|| Build.MODEL.contains("Emulator")
						|| Build.MODEL.contains("Android SDK built for x86"));
	}

	@Override
	protected void onPause() {
		super.onPause();

		if (rendererSet) {
			glSurfaceView.onPause();
		}

		JNIWrapper.on_pause();
	}
	/*
	public void onBackPressed() {
        Log.v("Eduardo", "oii");

        return;
    } 
	 */
	@Override
	protected void onResume() {
		super.onResume();

		if (rendererSet) {
			glSurfaceView.onResume();
		}

		JNIWrapper.on_resume();
	}

	public void showSoftKeyboard(){
		edittext.showKeyboard();
	}

	public void hideSoftKeyboard(){
		edittext.hideKeyboard();
	}
}
