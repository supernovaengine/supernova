package org.supernovaengine.supernova;

import static android.content.Context.DEVICE_POLICY_SERVICE;

import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;

public class EGLConfigChooser implements GLSurfaceView.EGLConfigChooser
{
    private int[] mConfigAttributes;
    private  final int EGL_OPENGL_ES2_BIT = 0x04;
    private  final int EGL_OPENGL_ES3_BIT = 0x40;
    public EGLConfigChooser(int redSize, int greenSize, int blueSize, int alphaSize, int depthSize, int stencilSize, int multisamplingCount)
    {
        mConfigAttributes = new int[] {redSize, greenSize, blueSize, alphaSize, depthSize, stencilSize, multisamplingCount};
    }
    public EGLConfigChooser(int[] attributes)
    {
        mConfigAttributes = attributes;
    }

    @Override
    public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display)
    {
        int[][] EGLAttributes = {
                {
                        // GL ES 3 with user set
                        EGL10.EGL_RED_SIZE, mConfigAttributes[0],
                        EGL10.EGL_GREEN_SIZE, mConfigAttributes[1],
                        EGL10.EGL_BLUE_SIZE, mConfigAttributes[2],
                        EGL10.EGL_ALPHA_SIZE, mConfigAttributes[3],
                        EGL10.EGL_DEPTH_SIZE, mConfigAttributes[4],
                        EGL10.EGL_STENCIL_SIZE, mConfigAttributes[5],
                        EGL10.EGL_SAMPLE_BUFFERS, (mConfigAttributes[6] > 0) ? 1 : 0,
                        EGL10.EGL_SAMPLES, mConfigAttributes[6],
                        EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                        EGL10.EGL_NONE
                },
                {
                        // GL ES 3 with user set 16 bit depth buffer
                        EGL10.EGL_RED_SIZE, mConfigAttributes[0],
                        EGL10.EGL_GREEN_SIZE, mConfigAttributes[1],
                        EGL10.EGL_BLUE_SIZE, mConfigAttributes[2],
                        EGL10.EGL_ALPHA_SIZE, mConfigAttributes[3],
                        EGL10.EGL_DEPTH_SIZE, mConfigAttributes[4] >= 24 ? 16 : mConfigAttributes[4],
                        EGL10.EGL_STENCIL_SIZE, mConfigAttributes[5],
                        EGL10.EGL_SAMPLE_BUFFERS, (mConfigAttributes[6] > 0) ? 1 : 0,
                        EGL10.EGL_SAMPLES, mConfigAttributes[6],
                        EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                        EGL10.EGL_NONE
                },
                {
                        // GL ES 3 with user set 16 bit depth buffer without multisampling
                        EGL10.EGL_RED_SIZE, mConfigAttributes[0],
                        EGL10.EGL_GREEN_SIZE, mConfigAttributes[1],
                        EGL10.EGL_BLUE_SIZE, mConfigAttributes[2],
                        EGL10.EGL_ALPHA_SIZE, mConfigAttributes[3],
                        EGL10.EGL_DEPTH_SIZE, mConfigAttributes[4] >= 24 ? 16 : mConfigAttributes[4],
                        EGL10.EGL_STENCIL_SIZE, mConfigAttributes[5],
                        EGL10.EGL_SAMPLE_BUFFERS, 0,
                        EGL10.EGL_SAMPLES, 0,
                        EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                        EGL10.EGL_NONE
                },
                {
                        // GL ES 3 by default
                        EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                        EGL10.EGL_NONE
                }
        };

        EGLConfig result = null;
        for (int[] eglAtribute : EGLAttributes) {
            result = this.doChooseConfig(egl, display, eglAtribute);
            if (result != null)
                return result;
        }

        Log.e(DEVICE_POLICY_SERVICE, "Can not select an EGLConfig for rendering.");
        return null;
    }

    private EGLConfig doChooseConfig(EGL10 egl, EGLDisplay display, int[] attributes) {
        EGLConfig[] configs = new EGLConfig[1];
        int[] matchedConfigNum = new int[1];
        boolean result = egl.eglChooseConfig(display, attributes, configs, 1, matchedConfigNum);
        if (result && matchedConfigNum[0] > 0) {
            return configs[0];
        }
        return null;
    }
}