package com.deslon.supernova;

import android.opengl.GLSurfaceView;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnKeyListener;

public class KeyListener implements OnKeyListener {
	
	private GLSurfaceView glSurfaceView;
	
	public KeyListener(GLSurfaceView glSurfaceView){
		this.glSurfaceView = glSurfaceView;
	}
	
	static private int convertToSupernova(int key){
		if (key == KeyEvent.KEYCODE_DPAD_UP)
			return 265;
		if (key == KeyEvent.KEYCODE_DPAD_DOWN)
			return 264;
		if (key == KeyEvent.KEYCODE_DPAD_RIGHT)
			return 262;
		if (key == KeyEvent.KEYCODE_DPAD_LEFT)
			return 263;		
		return 0;
	}

	@Override
	public boolean onKey(View v, final int keyCode, KeyEvent event) {
        if(event.getAction() == KeyEvent.ACTION_DOWN) {
            glSurfaceView.queueEvent(new Runnable() {
                @Override
                public void run() {
                	JNIWrapper.on_key_down(convertToSupernova(keyCode));
                }
            });
        }else if(event.getAction() == KeyEvent.ACTION_UP) {
            glSurfaceView.queueEvent(new Runnable() {
                @Override
                public void run() {
                	JNIWrapper.on_key_up(convertToSupernova(keyCode));
                }
            });
        }

        return true;
	}

}
