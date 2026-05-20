package com.racing.game;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;
import android.util.Log;

public class MainActivity extends Activity {

    private static final String TAG = "MainActivity";
    private GameGLView gameView;

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

        try {
            gameView = new GameGLView(this);
            setContentView(gameView);
        } catch (Exception e) {
            e.printStackTrace();
            finish();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (gameView != null) gameView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (gameView != null) gameView.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (gameView != null) gameView.onDestroy();
    }
}
