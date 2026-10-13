package org.doriaxengine.doriax;

import android.app.Activity;
import android.os.Build.VERSION;
import android.os.Build.VERSION_CODES;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager.LayoutParams;
import androidx.activity.OnBackPressedCallback;
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.core.view.WindowInsetsControllerCompat;

import com.google.androidgamesdk.GameActivity;

import java.util.ArrayList;
import java.util.List;

// A minimal extension of GameActivity. For this sample, it is only used to invoke
// a workaround for loading the runtime shared library on old Android versions
public class MainActivity extends GameActivity {

	// Load our native library:
	static {
		System.loadLibrary("doriax-android");
	}

	private UserSettings userSettings;
	// Null when the export leaves AdMob or Google Play Billing out
	private Object adMobWrapper;
	private Object billingWrapper;
	private final List<ActivityListener> activityListeners = new ArrayList<>();

	private native void onBackPressedNative();

	private void hideSystemUI() {
		// This will put the game behind any cutouts and waterfalls on devices which have
		// them, so the corresponding insets will be non-zero.
		if (VERSION.SDK_INT >= VERSION_CODES.P) {
			getWindow().getAttributes().layoutInDisplayCutoutMode
					= LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_ALWAYS;
		}
		// From API 30 onwards, this is the recommended way to hide the system UI, rather than
		// using View.setSystemUiVisibility.
		View decorView = getWindow().getDecorView();
		WindowInsetsControllerCompat controller = new WindowInsetsControllerCompat(getWindow(),
				decorView);
		controller.hide(WindowInsetsCompat.Type.systemBars());
		controller.hide(WindowInsetsCompat.Type.displayCutout());
		controller.setSystemBarsBehavior(
				WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
	}

	// The wrapper sources are compiled only when the export enables their service
	private Object createOptionalWrapper(String className) {
		try {
			Object wrapper = Class.forName(className).getConstructor(Activity.class).newInstance(this);
			if (wrapper instanceof ActivityListener) {
				activityListeners.add((ActivityListener) wrapper);
			}
			return wrapper;
		} catch (ReflectiveOperationException | LinkageError e) {
			return null;
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// When true, the app will fit inside any system UI windows.
		// When false, we render behind any system UI windows.
		WindowCompat.setDecorFitsSystemWindows(getWindow(), false);
		hideSystemUI();

		userSettings = new UserSettings(this);
		adMobWrapper = createOptionalWrapper("org.doriaxengine.doriax.AdMobWrapper");
		billingWrapper = createOptionalWrapper("org.doriaxengine.doriax.BillingWrapper");

		// Games get Back as Escape. A callback, since Android 16 no longer sends it as a key
		getOnBackPressedDispatcher().addCallback(this, new OnBackPressedCallback(true) {
			@Override
			public void handleOnBackPressed() {
				onBackPressedNative();
			}
		});

		super.onCreate(savedInstanceState);
	}

	@Override
	protected void onResume() {
		super.onResume();
		hideSystemUI();
		for (ActivityListener listener : activityListeners) {
			listener.onActivityResume();
		}
	}

	@Override
	protected void onPause() {
		for (ActivityListener listener : activityListeners) {
			listener.onActivityPause();
		}
		super.onPause();
	}

	@Override
	protected void onDestroy() {
		for (ActivityListener listener : activityListeners) {
			listener.onActivityDestroy();
		}
		super.onDestroy();
	}

	public UserSettings getUserSettings() {
		return userSettings;
	}

	public Object getAdMobWrapper() {
		return adMobWrapper;
	}

	public Object getBillingWrapper() {
		return billingWrapper;
	}
}
