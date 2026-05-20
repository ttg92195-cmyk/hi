package com.racing.game;

import android.app.Activity;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.view.Gravity;
import android.view.Window;
import android.view.WindowManager;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.ScrollView;
import android.widget.TextView;

import java.util.List;

/**
 * Leaderboard Screen - Shows top 20 scores from Firebase
 */
public class LeaderboardActivity extends Activity {

    private LinearLayout leaderboardLayout;
    private ProgressBar loadingBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(
            WindowManager.LayoutParams.FLAG_FULLSCREEN,
            WindowManager.LayoutParams.FLAG_FULLSCREEN
        );

        // Create layout programmatically (no XML needed)
        LinearLayout root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        root.setBackgroundColor(Color.parseColor("#1a1a2e"));
        root.setPadding(32, 48, 32, 32);

        // Title
        TextView title = new TextView(this);
        title.setText("LEADERBOARD");
        title.setTextColor(Color.parseColor("#FFD700"));
        title.setTextSize(28);
        title.setTypeface(null, Typeface.BOLD);
        title.setGravity(Gravity.CENTER);
        title.setPadding(0, 20, 0, 30);
        root.addView(title);

        // Player info
        FirebaseHelper fb = FirebaseHelper.getInstance();
        String playerName = fb.getPlayerName();
        if (playerName != null) {
            TextView playerInfo = new TextView(this);
            playerInfo.setText("You: " + playerName);
            playerInfo.setTextColor(Color.parseColor("#4FC3F7"));
            playerInfo.setTextSize(14);
            playerInfo.setGravity(Gravity.CENTER);
            playerInfo.setPadding(0, 0, 0, 20);
            root.addView(playerInfo);
        }

        // Loading bar
        loadingBar = new ProgressBar(this, null, android.R.attr.progressBarStyleHorizontal);
        loadingBar.setIndeterminate(true);
        loadingBar.setPadding(0, 10, 0, 20);
        root.addView(loadingBar);

        // ScrollView for leaderboard entries
        ScrollView scrollView = new ScrollView(this);
        leaderboardLayout = new LinearLayout(this);
        leaderboardLayout.setOrientation(LinearLayout.VERTICAL);
        leaderboardLayout.setPadding(0, 10, 0, 10);
        scrollView.addView(leaderboardLayout);
        root.addView(scrollView, new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            0, 1.0f
        ));

        // Back button hint
        TextView backHint = new TextView(this);
        backHint.setText("Tap anywhere to go back");
        backHint.setTextColor(Color.parseColor("#888888"));
        backHint.setTextSize(14);
        backHint.setGravity(Gravity.CENTER);
        backHint.setPadding(0, 20, 0, 10);
        root.addView(backHint);

        setContentView(root);

        // Load leaderboard data
        loadLeaderboard();
    }

    private void loadLeaderboard() {
        FirebaseHelper.getInstance().getLeaderboard(entries -> {
            runOnUiThread(() -> {
                loadingBar.setVisibility(ProgressBar.GONE);
                displayLeaderboard(entries);
            });
        });
    }

    private void displayLeaderboard(List<FirebaseHelper.LeaderboardEntry> entries) {
        leaderboardLayout.removeAllViews();

        if (entries.isEmpty()) {
            TextView empty = new TextView(this);
            empty.setText("No scores yet!\nPlay a game to be the first!");
            empty.setTextColor(Color.parseColor("#AAAAAA"));
            empty.setTextSize(16);
            empty.setGravity(Gravity.CENTER);
            empty.setPadding(0, 60, 0, 0);
            leaderboardLayout.addView(empty);
            return;
        }

        // Header row
        LinearLayout headerRow = createRow("#", "Player", "Score", "Coins", true);
        leaderboardLayout.addView(headerRow);

        // Entries
        for (int i = 0; i < entries.size(); i++) {
            FirebaseHelper.LeaderboardEntry entry = entries.get(i);
            String rank = String.valueOf(i + 1);

            // Special colors for top 3
            boolean isTop3 = i < 3;

            LinearLayout row = createRow(
                rank,
                entry.playerName,
                String.valueOf(entry.score),
                String.valueOf(entry.coins),
                isTop3
            );

            // Top 3 background colors
            if (i == 0) {
                row.setBackgroundColor(Color.parseColor("#4A350000")); // Gold tint
            } else if (i == 1) {
                row.setBackgroundColor(Color.parseColor("#4A000033")); // Silver tint
            } else if (i == 2) {
                row.setBackgroundColor(Color.parseColor("#4A001A00")); // Bronze tint
            }

            leaderboardLayout.addView(row);
        }
    }

    private LinearLayout createRow(String rank, String player, String score, String coins, boolean highlight) {
        LinearLayout row = new LinearLayout(this);
        row.setOrientation(LinearLayout.HORIZONTAL);
        row.setPadding(8, 12, 8, 12);

        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        );
        row.setLayoutParams(params);

        int textColor = highlight ? Color.parseColor("#FFD700") : Color.parseColor("#E0E0E0");
        float textSize = highlight ? 16f : 14f;

        // Rank
        TextView rankView = new TextView(this);
        rankView.setText(rank);
        rankView.setTextColor(textColor);
        rankView.setTextSize(textSize);
        rankView.setTypeface(null, highlight ? Typeface.BOLD : Typeface.NORMAL);
        rankView.setLayoutParams(new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 0.8f));
        rankView.setGravity(Gravity.CENTER);
        row.addView(rankView);

        // Player name
        TextView playerView = new TextView(this);
        playerView.setText(player);
        playerView.setTextColor(textColor);
        playerView.setTextSize(textSize);
        playerView.setTypeface(null, highlight ? Typeface.BOLD : Typeface.NORMAL);
        playerView.setLayoutParams(new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 2.5f));
        row.addView(playerView);

        // Score
        TextView scoreView = new TextView(this);
        scoreView.setText(score);
        scoreView.setTextColor(textColor);
        scoreView.setTextSize(textSize);
        scoreView.setTypeface(null, highlight ? Typeface.BOLD : Typeface.NORMAL);
        scoreView.setLayoutParams(new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 1.5f));
        scoreView.setGravity(Gravity.CENTER);
        row.addView(scoreView);

        // Coins
        TextView coinsView = new TextView(this);
        coinsView.setText(coins);
        coinsView.setTextColor(Color.parseColor("#FFD700"));
        coinsView.setTextSize(textSize);
        coinsView.setTypeface(null, highlight ? Typeface.BOLD : Typeface.NORMAL);
        coinsView.setLayoutParams(new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 1.2f));
        coinsView.setGravity(Gravity.CENTER);
        row.addView(coinsView);

        return row;
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        overridePendingTransition(0, 0);
    }

    @Override
    public boolean onTouchEvent(android.view.MotionEvent event) {
        if (event.getAction() == android.view.MotionEvent.ACTION_DOWN) {
            finish();
            overridePendingTransition(0, 0);
            return true;
        }
        return super.onTouchEvent(event);
    }
}
