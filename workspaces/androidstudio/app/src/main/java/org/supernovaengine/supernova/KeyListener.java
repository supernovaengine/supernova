package org.supernovaengine.supernova;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnKeyListener;

public class KeyListener implements OnKeyListener {

	// same from C++ code input.h
	private static final int S_MODIFIER_SHIFT          = 0x0001;
	private static final int S_MODIFIER_CONTROL        = 0x0002;
	private static final int S_MODIFIER_ALT            = 0x0004;
	private static final int S_MODIFIER_SUPER          = 0x0008;
	private static final int S_MODIFIER_CAPS_LOCK      = 0x0010;
	private static final int S_MODIFIER_NUM_LOCK       = 0x0020;

	private static final int S_KEY_UNKNOWN             = -1;
	private static final int S_KEY_SPACE               = 32;
	private static final int S_KEY_APOSTROPHE          = 39;  /* ' */
	private static final int S_KEY_COMMA               = 44;  /* , */
	private static final int S_KEY_MINUS               = 45;  /* - */
	private static final int S_KEY_PERIOD              = 46;  /* . */
	private static final int S_KEY_SLASH               = 47;  /* / */
	private static final int S_KEY_0                   = 48;
	private static final int S_KEY_1                   = 49;
	private static final int S_KEY_2                   = 50;
	private static final int S_KEY_3                   = 51;
	private static final int S_KEY_4                   = 52;
	private static final int S_KEY_5                   = 53;
	private static final int S_KEY_6                   = 54;
	private static final int S_KEY_7                   = 55;
	private static final int S_KEY_8                   = 56;
	private static final int S_KEY_9                   = 57;
	private static final int S_KEY_SEMICOLON           = 59;  /* ; */
	private static final int S_KEY_EQUAL               = 61;  /* = */
	private static final int S_KEY_A                   = 65;
	private static final int S_KEY_B                   = 66;
	private static final int S_KEY_C                   = 67;
	private static final int S_KEY_D                   = 68;
	private static final int S_KEY_E                   = 69;
	private static final int S_KEY_F                   = 70;
	private static final int S_KEY_G                   = 71;
	private static final int S_KEY_H                   = 72;
	private static final int S_KEY_I                   = 73;
	private static final int S_KEY_J                   = 74;
	private static final int S_KEY_K                   = 75;
	private static final int S_KEY_L                   = 76;
	private static final int S_KEY_M                   = 77;
	private static final int S_KEY_N                   = 78;
	private static final int S_KEY_O                   = 79;
	private static final int S_KEY_P                   = 80;
	private static final int S_KEY_Q                   = 81;
	private static final int S_KEY_R                   = 82;
	private static final int S_KEY_S                   = 83;
	private static final int S_KEY_T                   = 84;
	private static final int S_KEY_U                   = 85;
	private static final int S_KEY_V                   = 86;
	private static final int S_KEY_W                   = 87;
	private static final int S_KEY_X                   = 88;
	private static final int S_KEY_Y                   = 89;
	private static final int S_KEY_Z                   = 90;
	private static final int S_KEY_LEFT_BRACKET        = 91; /* [ */
	private static final int S_KEY_BACKSLASH           = 92;  /* \ */
	private static final int S_KEY_RIGHT_BRACKET       = 93;  /* ] */
	private static final int S_KEY_GRAVE_ACCENT        = 96; /* ` */
	private static final int S_KEY_WORLD_1             = 161; /* non-US #1 */
	private static final int S_KEY_WORLD_2             = 162; /* non-US #2 */
	private static final int S_KEY_ESCAPE              = 256;
	private static final int S_KEY_ENTER               = 257;
	private static final int S_KEY_TAB                 = 258;
	private static final int S_KEY_BACKSPACE           = 259;
	private static final int S_KEY_INSERT              = 260;
	private static final int S_KEY_DELETE              = 261;
	private static final int S_KEY_RIGHT               = 262;
	private static final int S_KEY_LEFT                = 263;
	private static final int S_KEY_DOWN                = 264;
	private static final int S_KEY_UP                  = 265;
	private static final int S_KEY_PAGE_UP             = 266;
	private static final int S_KEY_PAGE_DOWN           = 267;
	private static final int S_KEY_HOME                = 268;
	private static final int S_KEY_END                 = 269;
	private static final int S_KEY_CAPS_LOCK           = 280;
	private static final int S_KEY_SCROLL_LOCK         = 281;
	private static final int S_KEY_NUM_LOCK            = 282;
	private static final int S_KEY_PRINT_SCREEN        = 283;
	private static final int S_KEY_PAUSE               = 284;
	private static final int S_KEY_F1                  = 290;
	private static final int S_KEY_F2                  = 291;
	private static final int S_KEY_F3                  = 292;
	private static final int S_KEY_F4                  = 293;
	private static final int S_KEY_F5                  = 294;
	private static final int S_KEY_F6                  = 295;
	private static final int S_KEY_F7                  = 296;
	private static final int S_KEY_F8                  = 297;
	private static final int S_KEY_F9                  = 298;
	private static final int S_KEY_F10                 = 299;
	private static final int S_KEY_F11                 = 300;
	private static final int S_KEY_F12                 = 301;
	private static final int S_KEY_F13                 = 302;
	private static final int S_KEY_F14                 = 303;
	private static final int S_KEY_F15                 = 304;
	private static final int S_KEY_F16                 = 305;
	private static final int S_KEY_F17                 = 306;
	private static final int S_KEY_F18                 = 307;
	private static final int S_KEY_F19                 = 308;
	private static final int S_KEY_F20                 = 309;
	private static final int S_KEY_F21                 = 310;
	private static final int S_KEY_F22                 = 311;
	private static final int S_KEY_F23                 = 312;
	private static final int S_KEY_F24                 = 313;
	private static final int S_KEY_F25                 = 314;
	private static final int S_KEY_KP_0                = 320;
	private static final int S_KEY_KP_1                = 321;
	private static final int S_KEY_KP_2                = 322;
	private static final int S_KEY_KP_3                = 323;
	private static final int S_KEY_KP_4                = 324;
	private static final int S_KEY_KP_5                = 325;
	private static final int S_KEY_KP_6                = 326;
	private static final int S_KEY_KP_7                = 327;
	private static final int S_KEY_KP_8                = 328;
	private static final int S_KEY_KP_9                = 329;
	private static final int S_KEY_KP_DECIMAL          = 330;
	private static final int S_KEY_KP_DIVIDE           = 331;
	private static final int S_KEY_KP_MULTIPLY         = 332;
	private static final int S_KEY_KP_SUBTRACT         = 333;
	private static final int S_KEY_KP_ADD              = 334;
	private static final int S_KEY_KP_ENTER            = 335;
	private static final int S_KEY_KP_EQUAL            = 336;
	private static final int S_KEY_LEFT_SHIFT          = 340;
	private static final int S_KEY_LEFT_CONTROL        = 341;
	private static final int S_KEY_LEFT_ALT            = 342;
	private static final int S_KEY_LEFT_SUPER          = 343;
	private static final int S_KEY_RIGHT_SHIFT         = 344;
	private static final int S_KEY_RIGHT_CONTROL       = 345;
	private static final int S_KEY_RIGHT_ALT           = 346;
	private static final int S_KEY_RIGHT_SUPER         = 347;
	private static final int S_KEY_MENU                = 348;
	private static final int S_KEY_LAST                = S_KEY_MENU;

	private Activity activity;
	private GLSurfaceView glSurfaceView;
	
	public KeyListener(Activity activity, GLSurfaceView glSurfaceView){
		this.activity = activity;
		this.glSurfaceView = glSurfaceView;
	}
	
	static private int getSupernovaKey(int key){
		if (key == KeyEvent.KEYCODE_SPACE)
			return S_KEY_SPACE;
		if (key == KeyEvent.KEYCODE_APOSTROPHE)
			return S_KEY_APOSTROPHE;
		if (key == KeyEvent.KEYCODE_COMMA)
			return S_KEY_COMMA;
		if (key == KeyEvent.KEYCODE_MINUS)
			return S_KEY_MINUS;
		if (key == KeyEvent.KEYCODE_PERIOD)
			return S_KEY_PERIOD;
		if (key == KeyEvent.KEYCODE_SLASH)
			return S_KEY_SLASH;

		if (key >= KeyEvent.KEYCODE_0 && key <= KeyEvent.KEYCODE_9)
			return key + S_KEY_0 - KeyEvent.KEYCODE_0;

		if (key == KeyEvent.KEYCODE_SEMICOLON)
			return S_KEY_SEMICOLON;
		if (key == KeyEvent.KEYCODE_EQUALS)
			return S_KEY_EQUAL;

		if (key >= KeyEvent.KEYCODE_A && key <= KeyEvent.KEYCODE_Z)
			return key + S_KEY_A - KeyEvent.KEYCODE_A;

		if (key == KeyEvent.KEYCODE_LEFT_BRACKET)
			return S_KEY_LEFT_BRACKET;
		if (key == KeyEvent.KEYCODE_BACKSLASH)
			return S_KEY_BACKSLASH;
		if (key == KeyEvent.KEYCODE_RIGHT_BRACKET)
			return S_KEY_RIGHT_BRACKET;
		if (key == KeyEvent.KEYCODE_GRAVE)
			return S_KEY_GRAVE_ACCENT;

		if (key == KeyEvent.KEYCODE_ESCAPE)
			return S_KEY_ESCAPE;
		if (key == KeyEvent.KEYCODE_ENTER)
			return S_KEY_ENTER;
		if (key == KeyEvent.KEYCODE_TAB)
			return S_KEY_TAB;
		if (key == KeyEvent.KEYCODE_DEL)
			return S_KEY_BACKSPACE;
		if (key == KeyEvent.KEYCODE_INSERT)
			return S_KEY_INSERT;
		if (key == KeyEvent.KEYCODE_FORWARD_DEL)
			return S_KEY_DELETE;
		if (key == KeyEvent.KEYCODE_DPAD_RIGHT)
			return S_KEY_RIGHT;
		if (key == KeyEvent.KEYCODE_DPAD_LEFT)
			return S_KEY_LEFT;
		if (key == KeyEvent.KEYCODE_DPAD_DOWN)
			return S_KEY_DOWN;
		if (key == KeyEvent.KEYCODE_DPAD_UP)
			return S_KEY_UP;
		if (key == KeyEvent.KEYCODE_PAGE_UP)
			return S_KEY_PAGE_UP;
		if (key == KeyEvent.KEYCODE_PAGE_DOWN)
			return S_KEY_PAGE_DOWN;
		if (key == KeyEvent.KEYCODE_MOVE_HOME)
			return S_KEY_HOME;
		if (key == KeyEvent.KEYCODE_MOVE_END)
			return S_KEY_END;
		if (key == KeyEvent.KEYCODE_CAPS_LOCK)
			return S_KEY_CAPS_LOCK;
		if (key == KeyEvent.KEYCODE_SCROLL_LOCK)
			return S_KEY_SCROLL_LOCK;
		if (key == KeyEvent.KEYCODE_NUM_LOCK)
			return S_KEY_NUM_LOCK;
		if (key == KeyEvent.KEYCODE_SYSRQ)
			return S_KEY_PRINT_SCREEN;
		if (key == KeyEvent.KEYCODE_BREAK)
			return S_KEY_PAUSE;

		if (key >= KeyEvent.KEYCODE_F1 && key <= KeyEvent.KEYCODE_F12)
			return key + S_KEY_F1 - KeyEvent.KEYCODE_F1;

		if (key >= KeyEvent.KEYCODE_NUMPAD_0 && key <= KeyEvent.KEYCODE_NUMPAD_9)
			return key + S_KEY_KP_0 - KeyEvent.KEYCODE_NUMPAD_0;
		if (key == KeyEvent.KEYCODE_NUMPAD_DOT)
			return S_KEY_KP_DECIMAL;
		if (key == KeyEvent.KEYCODE_NUMPAD_DIVIDE)
			return S_KEY_KP_DIVIDE;
		if (key == KeyEvent.KEYCODE_NUMPAD_MULTIPLY)
			return S_KEY_KP_MULTIPLY;
		if (key == KeyEvent.KEYCODE_NUMPAD_SUBTRACT)
			return S_KEY_KP_SUBTRACT;
		if (key == KeyEvent.KEYCODE_NUMPAD_ADD)
			return S_KEY_KP_ADD;
		if (key == KeyEvent.KEYCODE_NUMPAD_ENTER)
			return S_KEY_KP_ENTER;
		if (key == KeyEvent.KEYCODE_NUMPAD_EQUALS)
			return S_KEY_KP_EQUAL;

		if (key == KeyEvent.KEYCODE_SHIFT_LEFT)
			return S_KEY_LEFT_SHIFT;
		if (key == KeyEvent.KEYCODE_CTRL_LEFT)
			return S_KEY_LEFT_CONTROL;
		if (key == KeyEvent.KEYCODE_ALT_LEFT)
			return S_KEY_LEFT_ALT;
		if (key == KeyEvent.KEYCODE_META_LEFT)
			return S_KEY_LEFT_SUPER;
		if (key == KeyEvent.KEYCODE_SHIFT_RIGHT)
			return S_KEY_RIGHT_SHIFT;
		if (key == KeyEvent.KEYCODE_CTRL_RIGHT)
			return S_KEY_RIGHT_CONTROL;
		if (key == KeyEvent.KEYCODE_ALT_RIGHT)
			return S_KEY_RIGHT_ALT;
		if (key == KeyEvent.KEYCODE_META_RIGHT)
			return S_KEY_RIGHT_SUPER;
		if (key == KeyEvent.KEYCODE_MENU)
			return S_KEY_MENU;

		return 0;
	}

	static private int getModifiers(KeyEvent event){
		int modifiers = 0;
		if (event.isCtrlPressed()) modifiers |= S_MODIFIER_CONTROL;
		if (event.isShiftPressed()) modifiers |= S_MODIFIER_SHIFT;
		if (event.isAltPressed())  modifiers |= S_MODIFIER_ALT;
		if (event.isMetaPressed()) modifiers |= S_MODIFIER_SUPER;
		if (event.isCapsLockOn()) modifiers |= S_MODIFIER_CAPS_LOCK;
		if (event.isNumLockOn()) modifiers |= S_MODIFIER_NUM_LOCK;

		return modifiers;
	}

	@Override
	public boolean onKey(View v, final int keyCode, KeyEvent event) {
		final int supernovaKey = getSupernovaKey(keyCode);
		final int modifiers = getModifiers(event);
		final boolean repeat = (event.getRepeatCount() > 0)?true:false;

        if(event.getAction() == KeyEvent.ACTION_DOWN && supernovaKey > 0) {
            glSurfaceView.queueEvent(new Runnable() {
                @Override
                public void run() {
                	JNIWrapper.system_key_down(supernovaKey, repeat, modifiers);
                }
            });
            return true;
        }else if(event.getAction() == KeyEvent.ACTION_UP && supernovaKey > 0) {
            glSurfaceView.queueEvent(new Runnable() {
                @Override
                public void run() {
                	JNIWrapper.system_key_up(supernovaKey, repeat, modifiers);
                }
            });
            return true;
        }

        return false;
	}

}
