package com.racing.game;

import android.app.Activity;
import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.FrameLayout;
import android.view.Window;
import android.view.WindowManager;
import android.util.Log;

public class MainActivity extends Activity implements GameUIOverlay.GameUIListener {

    private static final String TAG = "MainActivity";
    private GameGLView gameView;
    private GameUIOverlay uiOverlay;
    private SoundManager soundManager;
    private Handler uiHandler;
    private boolean gameRunning = false;

    // Track previous state for sound triggers
    private int prevLives = 3;
    private int prevCoins = 0;
    private boolean prevGameOver = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(
            WindowManager.LayoutParams.FLAG_FULLSCREEN,
            WindowManager.LayoutParams.FLAG_FULLSCREEN
        );
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // Initialize Firebase
        try {
            FirebaseHelper.getInstance().init();
            Log.i(TAG, "Firebase initialized");
        } catch (Exception e) {
            Log.e(TAG, "Firebase init error: " + e.getMessage());
        }

        // Initialize Sound Manager
        soundManager = new SoundManager(this);
        soundManager.init();

        // Create GL Surface View
        gameView = new GameGLView(this);

        // Create UI Overlay
        uiOverlay = new GameUIOverlay(this, this);

        // Layout: GL view as base, UI overlay on top
        FrameLayout rootLayout = new FrameLayout(this);
        rootLayout.addView(gameView);
        rootLayout.addView(uiOverlay.getRootView());

        setContentView(rootLayout);

        uiHandler = new Handler(Looper.getMainLooper());

        // Start UI update loop
        startUIUpdateLoop();
    }

    /**
     * Periodically update UI with game state
     */
    private void startUIUpdateLoop() {
        uiHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                if (gameView != null && gameView.getRenderer() != null) {
                    GameRenderer r = gameView.getRenderer();
                    int score = r.nativeGetScore();
                    int coins = r.nativeGetCoins();
                    int lives = r.nativeGetLives();
                    int speed = r.nativeGetSpeedKmh();
                    boolean shield = r.nativeHasShield();
                    boolean gameOver = r.nativeIsGameOver();

                    // Update HUD
                    if (uiOverlay.getState() == GameUIOverlay.GameState.PLAYING) {
                        uiOverlay.updateHUD(score, coins, lives, speed, shield, true);

                        // Update engine sound pitch
                        soundManager.updateEngineSpeed(speed);

                        // Detect coin pickup
                        if (coins > prevCoins) {
                            soundManager.playCoinSound();
                        }
                        prevCoins = coins;

                        // Detect hit (lives decreased)
                        if (lives < prevLives) {
                            soundManager.playCrashSound();
                        }
                        prevLives = lives;

                        // Detect game over
                        if (gameOver && !prevGameOver) {
                            soundManager.playGameOverSound();
                            soundManager.stopEngine();
                            uiOverlay.updateGameOver(score, r.nativeGetHighScore(), coins);
                            uiOverlay.setState(GameUIOverlay.GameState.GAME_OVER);

                            // Submit score to Firebase
                            submitScore(score, coins);
                        }
                        prevGameOver = gameOver;
                    }
                }
                uiHandler.postDelayed(this, 50); // Update every 50ms (20fps for UI)
            }
        }, 100);
    }

    /**
     * Submit score to Firebase
     */
    private void submitScore(int score, int coins) {
        FirebaseHelper fb = FirebaseHelper.getInstance();
        if (fb.isSignedIn()) {
            fb.submitScore(score, coins, new FirebaseHelper.ScoreCallback() {
                @Override
                public void onSuccess() {
                    Log.i(TAG, "Score submitted: " + score);
                }
                @Override
                public void onError(String message) {
                    Log.e(TAG, "Score submit failed: " + message);
                }
            });
        }
    }

    // ====== GameUIListener Implementation ======

    @Override
    public void onStartGame() {
        soundManager.playClickSound();
        uiOverlay.setState(GameUIOverlay.GameState.PLAYING);
        gameRunning = true;
        prevLives = 3;
        prevCoins = 0;
        prevGameOver = false;
        soundManager.startEngine();
    }

    @Override
    public void onResumeGame() {
        soundManager.playClickSound();
        uiOverlay.setState(GameUIOverlay.GameState.PLAYING);
        if (gameView != null && gameView.getRenderer() != null) {
            gameView.getRenderer().nativePause(); // toggle pause off
        }
        soundManager.startEngine();
    }

    @Override
    public void onRestartGame() {
        soundManager.playClickSound();
        if (gameView != null && gameView.getRenderer() != null) {
            gameView.getRenderer().nativeRestart();
        }
        uiOverlay.setState(GameUIOverlay.GameState.PLAYING);
        prevLives = 3;
        prevCoins = 0;
        prevGameOver = false;
        gameRunning = true;
        soundManager.startEngine();
    }

    @Override
    public void onOpenLeaderboard() {
        soundManager.playClickSound();
        try {
            Intent intent = new Intent(this, LeaderboardActivity.class);
            startActivity(intent);
        } catch (Exception e) {
            Log.e(TAG, "Failed to open leaderboard: " + e.getMessage());
        }
    }

    @Override
    public void onOpenSettings() {
        soundManager.playClickSound();
        // Toggle sound
        soundManager.setSoundEnabled(!soundManager.isSoundEnabled());
    }

    @Override
    public void onNitro() {
        if (gameView != null && gameView.getRenderer() != null) {
            gameView.getRenderer().nativeNitro();
        }
        soundManager.playNitroSound();
    }

    @Override
    public void onPause() {
        soundManager.playClickSound();
        if (gameView != null && gameView.getRenderer() != null) {
            gameView.getRenderer().nativePause();
        }
        uiOverlay.setState(GameUIOverlay.GameState.PAUSED);
        soundManager.stopEngine();
    }

    @Override
    public void onToggleSound() {
        soundManager.setSoundEnabled(!soundManager.isSoundEnabled());
    }

    // ====== Activity Lifecycle ======

    @Override
    protected void onPause() {
        super.onPause();
        if (gameView != null) gameView.onPause();
        soundManager.stopEngine();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (gameView != null) gameView.onResume();
        if (gameRunning && uiOverlay.getState() == GameUIOverlay.GameState.PLAYING) {
            soundManager.startEngine();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (gameView != null) gameView.onDestroy();
        soundManager.release();
        uiHandler.removeCallbacksAndMessages(null);
    }
}
