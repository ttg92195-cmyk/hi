package com.racing.game;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.util.Log;

/**
 * Game GL Surface View - Handles OpenGL rendering and touch input for steering
 * UI (menus, HUD, buttons) is handled by GameUIOverlay
 */
public class GameGLView extends GLSurfaceView {

    private static final String TAG = "GameGLView";
    private GameRenderer renderer;

    // Touch state for steering
    private float touchLastX = 0;
    private boolean touching = false;

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
        int w = getWidth();

        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                touchLastX = x;
                touching = true;
                break;

            case MotionEvent.ACTION_MOVE:
                if (touching) {
                    float dx = x - touchLastX;
                    float dt = 0.016f;

                    if (dx < -3) {
                        renderer.nativeSteerLeft(dt);
                    } else if (dx > 3) {
                        renderer.nativeSteerRight(dt);
                    }

                    // Set absolute position based on touch
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

    public GameRenderer getRenderer() {
        return renderer;
    }

    public void onDestroy() {
        if (renderer != null) {
            renderer.nativeDestroy();
        }
    }
}
