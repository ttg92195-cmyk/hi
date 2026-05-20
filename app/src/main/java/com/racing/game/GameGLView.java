package com.racing.game;

import android.content.Context;
import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.util.Log;

public class GameGLView extends GLSurfaceView {

    private static final String TAG = "GameGLView";

    private GameRenderer renderer;
    private Context context;

    // Touch state
    private float touchStartX = 0;
    private float touchLastX = 0;
    private boolean touching = false;
    private long touchDownTime = 0;

    // Track if score was already submitted for current game
    private boolean scoreSubmitted = false;

    public GameGLView(Context context) {
        super(context);
        this.context = context;
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

                // Game Over screen buttons
                if (renderer.nativeIsGameOver()) {
                    // Restart button (center)
                    float btnW = w * 0.35f;
                    float btnH = h * 0.1f;
                    float btnX = w / 2f - btnW / 2;
                    float btnY = h / 2f + h * 0.15f;
                    if (x >= btnX && x <= btnX + btnW && y >= btnY && y <= btnY + btnH) {
                        // Submit score before restart
                        submitScoreIfNotYet();
                        renderer.nativeRestart();
                        scoreSubmitted = false;
                        return true;
                    }

                    // Leaderboard button (bottom-left on game over)
                    float lbW = w * 0.25f;
                    float lbH = h * 0.08f;
                    float lbX = w * 0.05f;
                    float lbY = h / 2f + h * 0.25f;
                    if (x >= lbX && x <= lbX + lbW && y >= lbY && y <= lbY + lbH) {
                        submitScoreIfNotYet();
                        openLeaderboard();
                        return true;
                    }

                    return true;
                }

                // Leaderboard button (bottom-left, always visible)
                float lbR = Math.min(w, h) * 0.06f;
                float lbX = lbR * 1.5f;
                float lbY = h - lbR * 1.5f;
                if (Math.sqrt((x - lbX) * (x - lbX) + (y - lbY) * (y - lbY)) < lbR) {
                    openLeaderboard();
                    return true;
                }

                // Nitro button (bottom right)
                float nbR = Math.min(w, h) * 0.07f;
                float nbX = w - nbR * 1.5f;
                float nbY = h - nbR * 1.5f;
                if (Math.sqrt((x - nbX) * (x - nbX) + (y - nbY) * (y - nbY)) < nbR) {
                    renderer.nativeNitro();
                    return true;
                }

                // Pause button (top right)
                float pbR = Math.min(w, h) * 0.05f;
                float pbX = w - pbR * 1.5f;
                float pbY = pbR * 1.5f;
                if (Math.sqrt((x - pbX) * (x - pbX) + (y - pbY) * (y - pbY)) < pbR) {
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

    /**
     * Submit score to Firebase if not already submitted
     */
    private void submitScoreIfNotYet() {
        if (scoreSubmitted) return;
        scoreSubmitted = true;

        int score = renderer.nativeGetScore();
        int coins = renderer.nativeGetCoins();

        FirebaseHelper fb = FirebaseHelper.getInstance();
        if (fb.isSignedIn()) {
            fb.submitScore(score, coins, new FirebaseHelper.ScoreCallback() {
                @Override
                public void onSuccess() {
                    Log.i(TAG, "Score submitted to Firebase: " + score);
                }

                @Override
                public void onError(String message) {
                    Log.e(TAG, "Score submit failed: " + message);
                }
            });
        }
    }

    /**
     * Open the Leaderboard screen
     */
    private void openLeaderboard() {
        try {
            Intent intent = new Intent(context, LeaderboardActivity.class);
            context.startActivity(intent);
        } catch (Exception e) {
            Log.e(TAG, "Failed to open leaderboard: " + e.getMessage());
        }
    }

    public void onDestroy() {
        // Submit score on destroy if game was over
        if (renderer != null && renderer.nativeIsGameOver()) {
            submitScoreIfNotYet();
        }
        if (renderer != null) {
            renderer.nativeDestroy();
        }
    }
}
