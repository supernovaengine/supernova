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
import android.content.SharedPreferences;


@SuppressLint("ClickableViewAccessibility")
public class MainActivity extends Activity {
    
	private GLSurfaceView glSurfaceView;
	private boolean rendererSet;

	private FrameLayout layout;
	private TextInput edittext;

	private static final String PREFS_FILE_NAME = "UserSettings";

	public int screenWidth;
	public int screenHeight;
    
    @Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		requestWindowFeature(Window.FEATURE_NO_TITLE);
	    getWindow().setFlags(
	            WindowManager.LayoutParams.FLAG_FULLSCREEN,
	            WindowManager.LayoutParams.FLAG_FULLSCREEN);

		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

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

			screenWidth = dm.widthPixels;
			screenWidth = dm.heightPixels;

			final RendererWrapper rendererWrapper = new RendererWrapper(MainActivity.this, this.getAssets());
			
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
			//edittext.setRawInputType(InputType.TYPE_CLASS_TEXT);
			edittext.setImeOptions(EditorInfo.IME_ACTION_DONE | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
			edittext.setSingleLine(true);
			layout.addView(edittext);
			//addContentView(edittext, new ViewGroup.LayoutParams(LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));

			edittext.setOnEditorActionListener(new TextView.OnEditorActionListener() {
				@Override
				public boolean onEditorAction(TextView textView, int id, KeyEvent keyEvent) {
                    if (id == EditorInfo.IME_ACTION_NEXT) {
                        hideSoftKeyboard();
                        return true;
                    } else if (id == EditorInfo.IME_ACTION_DONE || id == EditorInfo.IME_ACTION_SEND || id == EditorInfo.IME_ACTION_SEARCH || id == EditorInfo.IME_ACTION_GO) {
                        hideSoftKeyboard();
                    }
                    return false;
				}
			});

			edittext.setOnKeyListener(new View.OnKeyListener() {
				@Override
				public boolean onKey(View v, int keyCode, KeyEvent event) {
					if ((event.getAction() == KeyEvent.ACTION_DOWN) && (keyCode == KeyEvent.KEYCODE_ENTER)) {
						hideSoftKeyboard();
					}
					return false;
				}
			});

			layout.addView(glSurfaceView);
			edittext.setView(glSurfaceView);
			
			glSurfaceView.setOnKeyListener(new KeyListener(this, glSurfaceView));
			
			glSurfaceView.setOnTouchListener(new OnTouchListener() {
				@Override
				public boolean onTouch(View v, MotionEvent event) {
	                if (event != null) {

						int pointerIndex = event.getActionIndex();

						final int pointerId = event.getPointerId(pointerIndex);
	                    final float normalizedX = (event.getX(pointerIndex) / (float) v.getWidth()) * 2 - 1;
	                    final float normalizedY = (event.getY(pointerIndex) / (float) v.getHeight()) * 2 - 1;

						switch (event.getActionMasked()) {
							case MotionEvent.ACTION_DOWN:
							case MotionEvent.ACTION_POINTER_DOWN: {
								glSurfaceView.queueEvent(new Runnable() {
									@Override
									public void run() {
										JNIWrapper.system_touch_start(pointerId, normalizedX, normalizedY);
									}
								});
								break;
							}
							case MotionEvent.ACTION_UP:
							case MotionEvent.ACTION_POINTER_UP: {
								glSurfaceView.queueEvent(new Runnable() {
									@Override
									public void run() {
										JNIWrapper.system_touch_end(pointerId, normalizedX, normalizedY);
									}
								});
								break;
							}
							case MotionEvent.ACTION_MOVE: {
								glSurfaceView.queueEvent(new Runnable() {
									@Override
									public void run() {
										JNIWrapper.system_touch_move(pointerId, normalizedX, normalizedY);
									}
								});
								break;
							}
							case MotionEvent.ACTION_CANCEL: {
								glSurfaceView.queueEvent(new Runnable() {
									@Override
									public void run() { JNIWrapper.system_touch_cancel(); }
								});
								break;
							}
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

		JNIWrapper.system_pause();
	}

	@Override
	protected void onResume() {
		super.onResume();

		if (rendererSet) {
			glSurfaceView.onResume();
		}

		JNIWrapper.system_resume();
	}

	@Override
	public void onDestroy() {
    	// error when shutdown: call to OpenGL ES API with no current context (logged once per thread)
		//JNIWrapper.system_shutdown();

		super.onDestroy();
	}

	public void showSoftKeyboard(){
		edittext.showKeyboard();
	}

	public void hideSoftKeyboard(){
		edittext.hideKeyboard();
	}

	public int getScreenWidth(){ return screenWidth; }

	public int getScreenHeight(){ return screenHeight; }

	public String getUserDataPath() {
		return getFilesDir().getAbsolutePath();
	}

	public boolean getBoolForKey(String key, boolean defaultValue) {
		SharedPreferences sharedPref = getSharedPreferences(PREFS_FILE_NAME, Context.MODE_PRIVATE);

		try {
			return sharedPref.getBoolean(key, defaultValue);
		} catch (Exception ex) {
			ex.printStackTrace();
		}

		return defaultValue;
	}

	public int getIntegerForKey(String key, int defaultValue) {
		SharedPreferences sharedPref = getSharedPreferences(PREFS_FILE_NAME, Context.MODE_PRIVATE);

		try {
			return sharedPref.getInt(key, defaultValue);
		} catch (Exception ex) {
			ex.printStackTrace();
		}

		return defaultValue;
	}

	public long getLongForKey(String key, long defaultValue) {
		SharedPreferences sharedPref = getSharedPreferences(PREFS_FILE_NAME, Context.MODE_PRIVATE);

		try {
			return sharedPref.getLong(key, defaultValue);
		} catch (Exception ex) {
			ex.printStackTrace();
		}

		return defaultValue;
	}

	public float getFloatForKey(String key, float defaultValue) {
		SharedPreferences sharedPref = getSharedPreferences(PREFS_FILE_NAME, Context.MODE_PRIVATE);

		try {
			return sharedPref.getFloat(key, defaultValue);
		} catch (Exception ex) {
			ex.printStackTrace();
		}

		return defaultValue;
	}

	public double getDoubleForKey(String key, double defaultValue) {
		// SharedPreferences doesn't support double, using long to store
		SharedPreferences sharedPref = getSharedPreferences(PREFS_FILE_NAME, Context.MODE_PRIVATE);

		try {
			return Double.longBitsToDouble(sharedPref.getLong(key, Double.doubleToRawLongBits(defaultValue)));
		} catch (Exception ex) {
			ex.printStackTrace();
		}

		return defaultValue;
	}

	public String getStringForKey(String key, String defaultValue) {
		SharedPreferences sharedPref = getSharedPreferences(PREFS_FILE_NAME, Context.MODE_PRIVATE);

		try {
			return sharedPref.getString(key, defaultValue);
		} catch (Exception ex) {
			ex.printStackTrace();
		}

		return defaultValue;
	}

	public void setBoolForKey(String key, boolean value) {
		SharedPreferences sharedPref = getSharedPreferences(PREFS_FILE_NAME, Context.MODE_PRIVATE);
		SharedPreferences.Editor editor = sharedPref.edit();
		editor.putBoolean(key, value);
		editor.apply();
	}

	public void setIntegerForKey(String key, int value) {
		SharedPreferences sharedPref = getSharedPreferences(PREFS_FILE_NAME, Context.MODE_PRIVATE);
		SharedPreferences.Editor editor = sharedPref.edit();
		editor.putInt(key, value);
		editor.apply();
	}

	public void setLongForKey(String key, long value) {
		SharedPreferences sharedPref = getSharedPreferences(PREFS_FILE_NAME, Context.MODE_PRIVATE);
		SharedPreferences.Editor editor = sharedPref.edit();
		editor.putLong(key, value);
		editor.apply();
	}

	public void setFloatForKey(String key, float value) {
		SharedPreferences sharedPref = getSharedPreferences(PREFS_FILE_NAME, Context.MODE_PRIVATE);
		SharedPreferences.Editor editor = sharedPref.edit();
		editor.putFloat(key, value);
		editor.apply();
	}

	public void setDoubleForKey(String key, double value) {
		// SharedPreferences doesn't support double, using long to store
		SharedPreferences sharedPref = getSharedPreferences(PREFS_FILE_NAME, Context.MODE_PRIVATE);
		SharedPreferences.Editor editor = sharedPref.edit();
		editor.putLong(key, Double.doubleToRawLongBits(value));
		editor.apply();
	}

	public void setStringForKey(String key, String value) {
		SharedPreferences sharedPref = getSharedPreferences(PREFS_FILE_NAME, Context.MODE_PRIVATE);
		SharedPreferences.Editor editor = sharedPref.edit();
		editor.putString(key, value);
		editor.apply();
	}

	public void removeKey(String key) {
		SharedPreferences sharedPref = getSharedPreferences(PREFS_FILE_NAME, Context.MODE_PRIVATE);
		SharedPreferences.Editor editor = sharedPref.edit();
		editor.remove(key);
		editor.apply();
	}

}
