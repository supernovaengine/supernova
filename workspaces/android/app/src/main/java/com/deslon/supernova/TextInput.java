package com.deslon.supernova;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.widget.EditText;
import android.os.Handler;
import android.os.Message;
import android.view.inputmethod.InputMethodManager;
import android.view.inputmethod.EditorInfo;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;
import android.text.TextWatcher;
import android.text.Editable;

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
		this.setPadding(0, 0, 0, 0);
		this.setImeOptions(EditorInfo.IME_FLAG_NO_EXTRACT_UI);

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
						JNIWrapper.on_text_input(String.valueOf(s.charAt(before)));
					} else {
						JNIWrapper.on_text_input("\b");
					}
				}
			});
		}

	}

	@Override
	public void afterTextChanged(Editable s) {
		// TODO Auto-generated method stub

	}

	@Override
	public boolean onEditorAction(final TextView v, final int actionId, final KeyEvent event) {
		if (this == v && this.isFullScreenEdit()) {
			final String characters = event.getCharacters();

			view.queueEvent(new Runnable() {
				@Override
				public void run() {
					JNIWrapper.on_text_input(characters);
				}
			});
		}

		return false;
	}

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