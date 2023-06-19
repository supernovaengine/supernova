package org.supernovaengine.supernova;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.graphics.Insets;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowInsets;
import android.view.WindowManager;
import android.view.WindowMetrics;
import android.view.inputmethod.EditorInfo;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.FrameLayout;
import android.view.ViewGroup.LayoutParams;

import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.core.view.WindowInsetsControllerCompat;


@SuppressLint("ClickableViewAccessibility")
public class MainActivity extends Activity {

	private GLSurfaceView glSurfaceView;
	private boolean rendererSet;

	private FrameLayout layout;
	private TextInput edittext;
	private AdMobWrapper admobWrapper;
	private UserSettings userSettings;

	public int screenWidth;
	public int screenHeight;

	// need to check these suppressed methods after each new version by removing SuppressWarnings
	@SuppressWarnings("deprecation")
	private void calculateScreenSize(){
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
			WindowMetrics windowMetrics = getWindowManager().getCurrentWindowMetrics();
			Insets insets = windowMetrics.getWindowInsets()
					.getInsetsIgnoringVisibility(WindowInsets.Type.systemBars());

			screenWidth = windowMetrics.getBounds().width() - insets.left - insets.right;
			screenHeight = windowMetrics.getBounds().height() - insets.top - insets.bottom;
		} else {
			DisplayMetrics displayMetrics = new DisplayMetrics();
			getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);

			screenWidth = displayMetrics.widthPixels;
			screenHeight = displayMetrics.heightPixels;
		}
	}

	private void setFullScreen(){
		WindowInsetsControllerCompat windowInsetsController =
				WindowCompat.getInsetsController(getWindow(), getWindow().getDecorView());

		windowInsetsController.setSystemBarsBehavior(
				WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
		);

		windowInsetsController.hide(WindowInsetsCompat.Type.systemBars());
	}
    
    @Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		requestWindowFeature(Window.FEATURE_NO_TITLE);

		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		ActivityManager activityManager 
			= (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
		ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();

		final boolean supportsEs2 = configurationInfo.reqGlEsVersion >= 0x30000;

		admobWrapper = new AdMobWrapper(this);
		userSettings = new UserSettings(this);

		JNIWrapper.init_native(this, admobWrapper, userSettings, this.getAssets());

		if (supportsEs2) {
			glSurfaceView = new GLSurfaceView(this);

			EGLConfigChooser chooser = new EGLConfigChooser(8, 8, 8, 8, 24, 8, 4);
			glSurfaceView.setEGLConfigChooser(chooser);

			calculateScreenSize();

			final RendererWrapper rendererWrapper = new RendererWrapper(MainActivity.this);
			
			glSurfaceView.setEGLContextClientVersion(2);
			glSurfaceView.setPreserveEGLContextOnPause(true);
			glSurfaceView.setRenderer(rendererWrapper);
			glSurfaceView.setFocusableInTouchMode(true);

			rendererSet = true;
			//setContentView(glSurfaceView);

			layout = new FrameLayout(this);
			layout.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
			setContentView(layout);

			setFullScreen();

			edittext = new TextInput(this);

			layout.addView(edittext);
			layout.addView(glSurfaceView);
			//addContentView(edittext, new ViewGroup.LayoutParams(LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));

			// for accessibility alerts in Google Play Console
			glSurfaceView.setContentDescription("opengl view");

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
			edittext.setView(glSurfaceView);
			
			glSurfaceView.setOnKeyListener(new KeyListener(this, glSurfaceView));
			
			glSurfaceView.setOnTouchListener(new OnTouchListener() {
				@Override
				public boolean onTouch(View v, MotionEvent event) {
	                if (event != null) {

						int pointerIndex = event.getActionIndex();

						final int pointerId = event.getPointerId(pointerIndex);
						final float pointerX = event.getX(pointerIndex);
						final float pointerY = event.getY(pointerIndex);

						switch (event.getActionMasked()) {
							case MotionEvent.ACTION_DOWN:
							case MotionEvent.ACTION_POINTER_DOWN: {
								glSurfaceView.queueEvent(new Runnable() {
									@Override
									public void run() {
										JNIWrapper.system_touch_start(pointerId, pointerX, pointerY);
									}
								});
								break;
							}
							case MotionEvent.ACTION_UP:
							case MotionEvent.ACTION_POINTER_UP: {
								glSurfaceView.queueEvent(new Runnable() {
									@Override
									public void run() {
										JNIWrapper.system_touch_end(pointerId, pointerX, pointerY);
									}
								});
								break;
							}
							case MotionEvent.ACTION_MOVE: {
								glSurfaceView.queueEvent(new Runnable() {
									@Override
									public void run() {
										JNIWrapper.system_touch_move(pointerId, pointerX, pointerY);
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
			Toast.makeText(this, "This device does not support OpenGL ES 3.0.",
					Toast.LENGTH_LONG).show();
			return;
		}
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
    	// Android lifecycle preserves variables after onDestroy
		JNIWrapper.system_shutdown();

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

}
