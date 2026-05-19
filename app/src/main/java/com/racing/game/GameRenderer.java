package com.racing.game;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.graphics.PixelFormat;
import android.view.SurfaceHolder;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GameRenderer implements GLSurfaceView.Renderer {

    // ====== Native Methods ======
    public native void nativeInit();
    public native void nativeResize(int width, int height);
    public native void nativeRender();
    public native void nativeDestroy();

    public native void nativeSteerLeft(float dt);
    public native void nativeSteerRight(float dt);
    public native void nativeSetPlayerX(float x);
    public native void nativeNitro();
    public native void nativePause();
    public native void nativeRestart();

    public native int nativeGetScore();
    public native int nativeGetHighScore();
    public native int nativeGetCoins();
    public native int nativeGetLives();
    public native int nativeGetSpeedKmh();
    public native boolean nativeIsGameOver();
    public native boolean nativeIsPaused();
    public native boolean nativeHasShield();

    private static boolean nativeLoaded = false;

    static {
        try {
            System.loadLibrary("racing3d");
            nativeLoaded = true;
        } catch (UnsatisfiedLinkError e) {
            nativeLoaded = false;
            e.printStackTrace();
        }
    }

    private int screenWidth = 0;
    private int screenHeight = 0;

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        if (nativeLoaded) {
            nativeInit();
        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        screenWidth = width;
        screenHeight = height;
        if (nativeLoaded) {
            nativeResize(width, height);
        }
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        if (nativeLoaded) {
            nativeRender();
        }
    }

    // ====== HUD Drawing on overlay ======
    // Note: HUD is rendered in C++ via orthographic projection overlay
    // For simplicity, we draw HUD elements using a transparent overlay

    public int getScreenWidth() { return screenWidth; }
    public int getScreenHeight() { return screenHeight; }
}
