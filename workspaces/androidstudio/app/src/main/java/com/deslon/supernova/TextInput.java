package com.deslon.supernova;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.ViewGroup;
import android.widget.EditText;
import android.os.Handler;
import android.os.Message;
import android.view.inputmethod.InputMethodManager;
import android.view.inputmethod.EditorInfo;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;
import android.text.TextWatcher;
import android.text.Editable;

// As reference:
// see https://github.com/godotengine/godot/blob/master/platform/android/java/lib/src/org/godotengine/godot/input/GodotTextInputWrapper.java

public class TextInput extends EditText implements TextWatcher, OnEditorActionListener{

	private final static int HANDLER_OPEN_KEYBOARD = 2;
	private final static int HANDLER_CLOSE_KEYBOARD = 3;

	private GLSurfaceView view;
	private static Handler sHandler;


	public TextInput(final Context context) {
		super(context);
		this.initView();
	}

	public TextInput(final Context context, final AttributeSet attrs) {
		super(context, attrs);
		this.initView();
	}

	public TextInput(final Context context, final AttributeSet attrs, final int defStyle) {
		super(context, attrs, defStyle);
		this.initView();
	}

	protected void initView() {
		this.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));
		//this.setRawInputType(InputType.TYPE_CLASS_TEXT);
		this.setImeOptions(EditorInfo.IME_ACTION_DONE | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
		//this.setSingleLine(true);
		this.setPadding(0, 0, 0, 0);
		this.setImeOptions(EditorInfo.IME_FLAG_NO_EXTRACT_UI);
		getText().insert(this.getSelectionStart(), " ");

		sHandler = new Handler() {
			@Override
			public void handleMessage(final Message msg) {
				switch (msg.what) {
					case HANDLER_OPEN_KEYBOARD: {
						TextInput edit = (TextInput)msg.obj;

						if (edit.requestFocus()) {
							final InputMethodManager imm = (InputMethodManager)view.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
							imm.showSoftInput(edit, 0);
						}

					} break;

					case HANDLER_CLOSE_KEYBOARD: {
						TextInput edit = (TextInput)msg.obj;

						final InputMethodManager imm = (InputMethodManager)view.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
						imm.hideSoftInputFromWindow(edit.getWindowToken(), 0);

						edit.view.requestFocus();

					} break;
				}
			}
		};
	}

	public void setView(final GLSurfaceView view) {
		this.view = view;
		view.requestFocus();
	}

	private boolean isFullScreenEdit() {
		final TextInput textField = this;
		final InputMethodManager imm = (InputMethodManager)textField.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
		return imm.isFullscreenMode();
	}

	@Override
	public boolean onKeyDown(final int keyCode, final KeyEvent keyEvent) {
		super.onKeyDown(keyCode, keyEvent);

		//Prevent program from going to background
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			this.view.requestFocus();
		}

		return true;
	}

	@Override
	public void beforeTextChanged(CharSequence s, int start, int count, int after) {
		// TODO Auto-generated method stub
	}

	@Override
	public void onTextChanged(final CharSequence s, final int start, final int before, final int count) {

		if (view != null) {
			view.queueEvent(new Runnable() {
				@Override
				public void run() {
					if (before < count) {
						if (start != 0 || s.charAt(start + before) != ' '){
							JNIWrapper.system_char_input(s.charAt(start + before));
						}
					} else {
						JNIWrapper.system_char_input('\b');
					}
				}
			});
		}

		if (getText().toString().isEmpty()){
			getText().insert(this.getSelectionStart(), " ");
		}

	}

	@Override
	public void afterTextChanged(Editable s) {
		// TODO Auto-generated method stub
	}

	//KeyEvent getCharacters is no longer used by the input system.
	@Override
	public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
		return false;
	}
/*
	public boolean onEditorAction(final TextView v, final int actionId, final KeyEvent event) {
		if (this == v && this.isFullScreenEdit()) {
			final String characters = event.getCharacters();

			view.queueEvent(new Runnable() {
				@Override
				public void run() {
					for (int i = 0; i < characters.length(); i++) {
						final int ch = characters.codePointAt(i);
						JNIWrapper.system_char_input(ch);
					}
				}
			});
		}

		return false;
	}
*/
	public void showKeyboard() {
		final Message msg = new Message();
		msg.what = HANDLER_OPEN_KEYBOARD;
		msg.obj = this;
		sHandler.sendMessage(msg);
	}

	public void hideKeyboard() {
		final Message msg = new Message();
		msg.what = HANDLER_CLOSE_KEYBOARD;
		msg.obj = this;
		sHandler.sendMessage(msg);
	}
}