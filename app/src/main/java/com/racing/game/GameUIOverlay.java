package com.racing.game;

import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.drawable.GradientDrawable;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * Game UI Overlay - All in-game UI elements
 * - HUD: Speed, Score, Coins, Lives, Nitro button, Leaderboard button
 * - Menu Screen: Start, Leaderboard, Settings
 * - Pause Screen: Resume, Restart, Leaderboard
 * - Game Over Screen: Score, Best, Restart, Leaderboard
 */
public class GameUIOverlay {

    public enum GameState {
        MENU, PLAYING, PAUSED, GAME_OVER
    }

    private Context context;
    private GameState currentState = GameState.MENU;
    private GameUIListener listener;

    // UI Containers
    private FrameLayout rootOverlay;
    private FrameLayout menuScreen;
    private FrameLayout pauseScreen;
    private FrameLayout gameOverScreen;
    private LinearLayout hudLayout;

    // HUD elements
    private TextView speedText;
    private TextView scoreText;
    private TextView coinsText;
    private TextView livesText;
    private TextView nitroText;
    private TextView shieldText;
    private TextView pauseBtn;

    // Game Over elements
    private TextView finalScoreText;
    private TextView bestScoreText;
    private TextView finalCoinsText;

    public interface GameUIListener {
        void onStartGame();
        void onResumeGame();
        void onRestartGame();
        void onOpenLeaderboard();
        void onOpenSettings();
        void onNitro();
        void onPauseGame();
        void onToggleSound();
    }

    public GameUIOverlay(Context context, GameUIListener listener) {
        this.context = context;
        this.listener = listener;
        createUI();
    }

    private void createUI() {
        rootOverlay = new FrameLayout(context);
        rootOverlay.setLayoutParams(new FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT,
            FrameLayout.LayoutParams.MATCH_PARENT
        ));

        createHUD();
        createMenuScreen();
        createPauseScreen();
        createGameOverScreen();

        rootOverlay.addView(hudLayout);
        rootOverlay.addView(menuScreen);
        rootOverlay.addView(pauseScreen);
        rootOverlay.addView(gameOverScreen);

        updateUI();
    }

    // ====== HUD ======
    private void createHUD() {
        hudLayout = new LinearLayout(context);
        hudLayout.setOrientation(LinearLayout.VERTICAL);
        hudLayout.setPadding(16, 16, 16, 16);

        FrameLayout.LayoutParams hudParams = new FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT,
            FrameLayout.LayoutParams.MATCH_PARENT
        );
        hudLayout.setLayoutParams(hudParams);

        // Top row: Score + Coins + Pause
        LinearLayout topRow = new LinearLayout(context);
        topRow.setOrientation(LinearLayout.HORIZONTAL);
        topRow.setGravity(Gravity.TOP | Gravity.FILL_HORIZONTAL);

        scoreText = createHUDText("SCORE: 0", 16, Color.parseColor("#FFFFFF"));
        scoreText.setTypeface(null, Typeface.BOLD);
        topRow.addView(scoreText, new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 2f));

        coinsText = createHUDText("COINS: 0", 14, Color.parseColor("#FFD700"));
        topRow.addView(coinsText, new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 1.5f));

        livesText = createHUDText("LIVES: 3", 14, Color.parseColor("#FF4444"));
        topRow.addView(livesText, new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f));

        pauseBtn = createHUDText("II", 20, Color.parseColor("#FFFFFF"));
        pauseBtn.setGravity(Gravity.CENTER);
        pauseBtn.setOnClickListener(v -> {
            if (listener != null) listener.onPauseGame();
        });
        LinearLayout.LayoutParams pauseParams = new LinearLayout.LayoutParams(
            (int)(40 * context.getResources().getDisplayMetrics().density),
            (int)(40 * context.getResources().getDisplayMetrics().density),
            0.5f
        );
        pauseBtn.setBackgroundResource(R.drawable.pause_bg);
        pauseBtn.setLayoutParams(pauseParams);
        topRow.addView(pauseBtn);

        hudLayout.addView(topRow);

        // Middle: spacer
        View spacer = new View(context);
        spacer.setLayoutParams(new LinearLayout.LayoutParams(1, 0, 1f));
        hudLayout.addView(spacer);

        // Bottom row: Speed + Shield/Nitro indicators
        LinearLayout bottomRow = new LinearLayout(context);
        bottomRow.setOrientation(LinearLayout.HORIZONTAL);
        bottomRow.setGravity(Gravity.BOTTOM | Gravity.FILL_HORIZONTAL);

        speedText = createHUDText("0 km/h", 18, Color.parseColor("#00FF88"));
        speedText.setTypeface(null, Typeface.BOLD);
        bottomRow.addView(speedText, new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 2f));

        shieldText = createHUDText("SHIELD", 12, Color.parseColor("#4488FF"));
        shieldText.setVisibility(View.GONE);
        bottomRow.addView(shieldText, new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f));

        nitroText = createHUDText("NITRO", 14, Color.parseColor("#FF8800"));
        nitroText.setTypeface(null, Typeface.BOLD);
        nitroText.setGravity(Gravity.CENTER);
        nitroText.setBackgroundResource(R.drawable.nitro_bg);
        int nitroSize = (int)(55 * context.getResources().getDisplayMetrics().density);
        nitroText.setLayoutParams(new LinearLayout.LayoutParams(nitroSize, nitroSize, 0f));
        nitroText.setOnClickListener(v -> {
            if (listener != null) listener.onNitro();
        });
        bottomRow.addView(nitroText);

        // Leaderboard button
        TextView lbBtn = createHUDText("LB", 12, Color.parseColor("#FFD700"));
        lbBtn.setGravity(Gravity.CENTER);
        lbBtn.setBackgroundResource(R.drawable.leaderboard_bg);
        int lbSize = (int)(45 * context.getResources().getDisplayMetrics().density);
        lbBtn.setLayoutParams(new LinearLayout.LayoutParams(lbSize, lbSize, 0f));
        lbBtn.setOnClickListener(v -> {
            if (listener != null) listener.onOpenLeaderboard();
        });
        bottomRow.addView(lbBtn);

        hudLayout.addView(bottomRow);
    }

    private TextView createHUDText(String text, float size, int color) {
        TextView tv = new TextView(context);
        tv.setText(text);
        tv.setTextColor(color);
        tv.setTextSize(size);
        tv.setPadding(4, 4, 4, 4);
        return tv;
    }

    // ====== MENU SCREEN ======
    private void createMenuScreen() {
        menuScreen = new FrameLayout(context);
        menuScreen.setLayoutParams(new FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT,
            FrameLayout.LayoutParams.MATCH_PARENT
        ));
        menuScreen.setBackgroundColor(Color.parseColor("#CC000000"));

        LinearLayout layout = new LinearLayout(context);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setGravity(Gravity.CENTER);
        layout.setPadding(60, 0, 60, 0);

        // Title
        TextView title = new TextView(context);
        title.setText("3D RACING");
        title.setTextColor(Color.parseColor("#FFD700"));
        title.setTextSize(42);
        title.setTypeface(null, Typeface.BOLD);
        title.setGravity(Gravity.CENTER);
        title.setPadding(0, 0, 0, 40);
        layout.addView(title);

        // Subtitle
        TextView subtitle = new TextView(context);
        subtitle.setText("Championship Edition");
        subtitle.setTextColor(Color.parseColor("#AAAAAA"));
        subtitle.setTextSize(16);
        subtitle.setGravity(Gravity.CENTER);
        subtitle.setPadding(0, 0, 0, 50);
        layout.addView(subtitle);

        // Start button
        layout.addView(createMenuButton("START RACE", "#00CC66", v -> {
            if (listener != null) listener.onStartGame();
        }));

        // Leaderboard button
        layout.addView(createMenuButton("LEADERBOARD", "#FFD700", v -> {
            if (listener != null) listener.onOpenLeaderboard();
        }));

        // Settings button
        layout.addView(createMenuButton("SETTINGS", "#4488FF", v -> {
            if (listener != null) listener.onOpenSettings();
        }));

        menuScreen.addView(layout);
    }

    // ====== PAUSE SCREEN ======
    private void createPauseScreen() {
        pauseScreen = new FrameLayout(context);
        pauseScreen.setLayoutParams(new FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT,
            FrameLayout.LayoutParams.MATCH_PARENT
        ));
        pauseScreen.setBackgroundColor(Color.parseColor("#AA000000"));

        LinearLayout layout = new LinearLayout(context);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setGravity(Gravity.CENTER);
        layout.setPadding(60, 0, 60, 0);

        // Title
        TextView title = new TextView(context);
        title.setText("PAUSED");
        title.setTextColor(Color.parseColor("#FFFFFF"));
        title.setTextSize(36);
        title.setTypeface(null, Typeface.BOLD);
        title.setGravity(Gravity.CENTER);
        title.setPadding(0, 0, 0, 40);
        layout.addView(title);

        // Resume button
        layout.addView(createMenuButton("RESUME", "#00CC66", v -> {
            if (listener != null) listener.onResumeGame();
        }));

        // Restart button
        layout.addView(createMenuButton("RESTART", "#FF8800", v -> {
            if (listener != null) listener.onRestartGame();
        }));

        // Leaderboard button
        layout.addView(createMenuButton("LEADERBOARD", "#FFD700", v -> {
            if (listener != null) listener.onOpenLeaderboard();
        }));

        pauseScreen.addView(layout);
    }

    // ====== GAME OVER SCREEN ======
    private void createGameOverScreen() {
        gameOverScreen = new FrameLayout(context);
        gameOverScreen.setLayoutParams(new FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT,
            FrameLayout.LayoutParams.MATCH_PARENT
        ));
        gameOverScreen.setBackgroundColor(Color.parseColor("#CC111111"));

        LinearLayout layout = new LinearLayout(context);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setGravity(Gravity.CENTER);
        layout.setPadding(60, 0, 60, 0);

        // Title
        TextView title = new TextView(context);
        title.setText("GAME OVER");
        title.setTextColor(Color.parseColor("#FF4444"));
        title.setTextSize(36);
        title.setTypeface(null, Typeface.BOLD);
        title.setGravity(Gravity.CENTER);
        title.setPadding(0, 0, 0, 30);
        layout.addView(title);

        // Score
        finalScoreText = new TextView(context);
        finalScoreText.setText("Score: 0");
        finalScoreText.setTextColor(Color.parseColor("#FFFFFF"));
        finalScoreText.setTextSize(24);
        finalScoreText.setGravity(Gravity.CENTER);
        finalScoreText.setPadding(0, 0, 0, 10);
        layout.addView(finalScoreText);

        // Best
        bestScoreText = new TextView(context);
        bestScoreText.setText("Best: 0");
        bestScoreText.setTextColor(Color.parseColor("#FFD700"));
        bestScoreText.setTextSize(18);
        bestScoreText.setGravity(Gravity.CENTER);
        bestScoreText.setPadding(0, 0, 0, 10);
        layout.addView(bestScoreText);

        // Coins
        finalCoinsText = new TextView(context);
        finalCoinsText.setText("Coins: 0");
        finalCoinsText.setTextColor(Color.parseColor("#FFD700"));
        finalCoinsText.setTextSize(16);
        finalCoinsText.setGravity(Gravity.CENTER);
        finalCoinsText.setPadding(0, 0, 0, 30);
        layout.addView(finalCoinsText);

        // Restart button
        layout.addView(createMenuButton("PLAY AGAIN", "#00CC66", v -> {
            if (listener != null) listener.onRestartGame();
        }));

        // Leaderboard button
        layout.addView(createMenuButton("LEADERBOARD", "#FFD700", v -> {
            if (listener != null) listener.onOpenLeaderboard();
        }));

        // Main menu button
        layout.addView(createMenuButton("MAIN MENU", "#4488FF", v -> {
            setState(GameState.MENU);
        }));

        gameOverScreen.addView(layout);
    }

    // ====== Helper: Create styled button ======
    private Button createMenuButton(String text, String bgColor, View.OnClickListener clickListener) {
        Button btn = new Button(context);
        btn.setText(text);
        btn.setTextColor(Color.WHITE);
        btn.setTextSize(16);
        btn.setTypeface(null, Typeface.BOLD);
        btn.setAllCaps(false);

        GradientDrawable bg = new GradientDrawable();
        bg.setColor(Color.parseColor(bgColor));
        bg.setCornerRadius(25f);
        btn.setBackground(bg);

        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            (int)(55 * context.getResources().getDisplayMetrics().density)
        );
        params.setMargins(0, 8, 0, 8);
        btn.setLayoutParams(params);

        btn.setOnClickListener(clickListener);
        return btn;
    }

    // ====== Update UI based on game state ======
    public void updateUI() {
        boolean isPlaying = currentState == GameState.PLAYING;
        boolean isMenu = currentState == GameState.MENU;
        boolean isPaused = currentState == GameState.PAUSED;
        boolean isGameOver = currentState == GameState.GAME_OVER;

        hudLayout.setVisibility(isPlaying ? View.VISIBLE : View.GONE);
        menuScreen.setVisibility(isMenu ? View.VISIBLE : View.GONE);
        pauseScreen.setVisibility(isPaused ? View.VISIBLE : View.GONE);
        gameOverScreen.setVisibility(isGameOver ? View.VISIBLE : View.GONE);
    }

    // ====== Update HUD values ======
    public void updateHUD(int score, int coins, int lives, int speedKmh, boolean hasShield, boolean hasNitro) {
        if (scoreText != null) scoreText.setText("SCORE: " + score);
        if (coinsText != null) coinsText.setText("COINS: " + coins);
        if (livesText != null) livesText.setText("LIVES: " + lives);
        if (speedText != null) speedText.setText(speedKmh + " km/h");
        if (shieldText != null) shieldText.setVisibility(hasShield ? View.VISIBLE : View.GONE);
        if (nitroText != null) {
            nitroText.setAlpha(hasNitro ? 1.0f : 0.4f);
        }
    }

    public void updateGameOver(int score, int bestScore, int coins) {
        if (finalScoreText != null) finalScoreText.setText("Score: " + score);
        if (bestScoreText != null) bestScoreText.setText("Best: " + bestScore);
        if (finalCoinsText != null) finalCoinsText.setText("Coins: " + coins);
    }

    // ====== State management ======
    public void setState(GameState state) {
        this.currentState = state;
        updateUI();
    }

    public GameState getState() { return currentState; }

    public View getRootView() { return rootOverlay; }
}
