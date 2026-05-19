package com.racing.game;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;

public class GameGLView extends GLSurfaceView {

    private GameRenderer renderer;

    // Touch state
    private float touchStartX = 0;
    private float touchLastX = 0;
    private boolean touching = false;
    private long touchDownTime = 0;

    public GameGLView(Context context) {
        super(context);
        setEGLContextClientVersion(2);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);

        renderer = new GameRenderer();
        setRenderer(renderer);
        setRenderMode(RENDERMODE_CONTINUOUSLY);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        float x = event.getX();
        float y = event.getY();
        int w = getWidth();
        int h = getHeight();

        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                touchStartX = x;
                touchLastX = x;
                touching = true;
                touchDownTime = System.currentTimeMillis();

                // Game Over restart button (center of screen)
                if (renderer.nativeIsGameOver()) {
                    float btnW = w * 0.35f;
                    float btnH = h * 0.1f;
                    float btnX = w / 2f - btnW / 2;
                    float btnY = h / 2f + h * 0.15f;
                    if (x >= btnX && x <= btnX + btnW && y >= btnY && y <= btnY + btnH) {
                        renderer.nativeRestart();
                        return true;
                    }
                }

                // Nitro button (bottom right)
                float nbR = Math.min(w, h) * 0.07f;
                float nbX = w - nbR * 1.5f;
                float nbY = h - nbR * 1.5f;
                if (Math.sqrt((x-nbX)*(x-nbX) + (y-nbY)*(y-nbY)) < nbR) {
                    renderer.nativeNitro();
                    return true;
                }

                // Pause button (top right)
                float pbR = Math.min(w, h) * 0.05f;
                float pbX = w - pbR * 1.5f;
                float pbY = pbR * 1.5f;
                if (Math.sqrt((x-pbX)*(x-pbX) + (y-pbY)*(y-pbY)) < pbR) {
                    renderer.nativePause();
                    return true;
                }

                // Tap to unpause
                if (renderer.nativeIsPaused()) {
                    renderer.nativePause();
                    return true;
                }
                break;

            case MotionEvent.ACTION_MOVE:
                if (touching && !renderer.nativeIsGameOver() && !renderer.nativeIsPaused()) {
                    float dx = x - touchLastX;
                    float dt = 0.016f; // ~60fps

                    if (dx < -3) {
                        renderer.nativeSteerLeft(dt);
                    } else if (dx > 3) {
                        renderer.nativeSteerRight(dt);
                    }

                    // Also set absolute position based on touch
                    float normX = (x / w - 0.5f) * 12.0f;
                    renderer.nativeSetPlayerX(normX);

                    touchLastX = x;
                }
                break;

            case MotionEvent.ACTION_UP:
                touching = false;
                break;
        }

        return true;
    }

    public void onDestroy() {
        if (renderer != null) {
            renderer.nativeDestroy();
        }
    }
}
