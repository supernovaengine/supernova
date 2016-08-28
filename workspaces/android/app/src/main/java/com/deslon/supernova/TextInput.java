package com.deslon.supernova;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.view.KeyEvent;
import android.view.WindowManager;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.TextView;

public class TextInput implements TextWatcher, TextView.OnEditorActionListener{
	private Activity activity;
	private AlertDialog.Builder builder;
	private AlertDialog dialog;
	private EditText input;
	private String prevBuffer;
	
	public TextInput(Activity activity, String prevBuffer){
		this.activity = activity;
		this.prevBuffer = prevBuffer;
	}
	
	public void show(){
		builder = new AlertDialog.Builder(activity);
		//builder.setTitle("Title");
		
		input = new EditText(activity);
		input.setText(prevBuffer);
		input.setInputType(InputType.TYPE_CLASS_TEXT);
		builder.setView(input);
		
		
		
		input.addTextChangedListener(this);
		input.setOnEditorActionListener(this);
		
		builder.setPositiveButton("OK", new DialogInterface.OnClickListener() { 
		    @Override
		    public void onClick(DialogInterface dialog, int which) {
		    	dialog.dismiss();
		    }
		});
		/*
		builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
		    @Override
		    public void onClick(DialogInterface dialog, int which) {
		        dialog.cancel();
		    }
		});
		*/
		
		activity.runOnUiThread(new Runnable() {
		    @Override
		    public void run() {
		    	
		    	dialog = builder.create();
				//InputMethodManager imm =  (InputMethodManager)activity.getSystemService(Context.INPUT_METHOD_SERVICE);
				//imm.toggleSoftInput(InputMethodManager.SHOW_FORCED,InputMethodManager.HIDE_IMPLICIT_ONLY);
				dialog.getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_VISIBLE);

		    	dialog.show();
		    	
		    	input.requestFocus();
		    	
		    }
		});
	}
	
	public void cancel(){
		activity.runOnUiThread(new Runnable() {
		    @Override
		    public void run() {
		    	dialog.cancel();
		    }
		});		
	}

	@Override
	public void beforeTextChanged(CharSequence s, int start, int count, int after) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onTextChanged(CharSequence s, int start, int before, int count) {
		if (before < count){
			JNIWrapper.on_text_input(String.valueOf(s.charAt(before)));
		}else{
			JNIWrapper.on_text_input("\b");
		}	
		
	}

	@Override
	public void afterTextChanged(Editable s) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
        if (actionId == EditorInfo.IME_ACTION_DONE) {
          	dialog.cancel();
          	JNIWrapper.on_text_input("\b");
        }
        return false;
	}
}
