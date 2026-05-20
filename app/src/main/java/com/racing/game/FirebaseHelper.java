package com.racing.game;

import android.util.Log;

import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.FirebaseUser;
import com.google.firebase.firestore.FirebaseFirestore;
import com.google.firebase.firestore.Query;
import com.google.firebase.firestore.QueryDocumentSnapshot;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Firebase Helper - Handles Authentication, Leaderboard, and Score saving
 * Uses Anonymous sign-in + Firestore for data storage
 */
public class FirebaseHelper {

    private static final String TAG = "FirebaseHelper";
    private static final String COLLECTION_LEADERBOARD = "leaderboard";
    private static final String COLLECTION_PLAYERS = "players";

    private static FirebaseHelper instance;

    private FirebaseAuth mAuth;
    private FirebaseFirestore db;
    private boolean initialized = false;
    private boolean signedIn = false;
    private String playerId = null;
    private String playerName = null;

    // Callback interface for async operations
    public interface LeaderboardCallback {
        void onResult(List<LeaderboardEntry> entries);
    }

    public interface ScoreCallback {
        void onSuccess();
        void onError(String message);
    }

    // Leaderboard entry data class
    public static class LeaderboardEntry {
        public String playerName;
        public int score;
        public int coins;
        public long timestamp;

        public LeaderboardEntry(String playerName, int score, int coins, long timestamp) {
            this.playerName = playerName;
            this.score = score;
            this.coins = coins;
            this.timestamp = timestamp;
        }
    }

    private FirebaseHelper() {}

    public static synchronized FirebaseHelper getInstance() {
        if (instance == null) {
            instance = new FirebaseHelper();
        }
        return instance;
    }

    /**
     * Initialize Firebase Auth and Firestore
     */
    public void init() {
        try {
            mAuth = FirebaseAuth.getInstance();
            db = FirebaseFirestore.getInstance();
            initialized = true;

            // Check if already signed in
            FirebaseUser currentUser = mAuth.getCurrentUser();
            if (currentUser != null) {
                playerId = currentUser.getUid();
                signedIn = true;
                Log.i(TAG, "Already signed in: " + playerId);
            } else {
                signInAnonymously();
            }
        } catch (Exception e) {
            Log.e(TAG, "Firebase init failed: " + e.getMessage());
            initialized = false;
        }
    }

    /**
     * Anonymous sign-in - no user input required
     */
    private void signInAnonymously() {
        if (!initialized || mAuth == null) return;

        mAuth.signInAnonymously()
            .addOnCompleteListener(task -> {
                if (task.isSuccessful()) {
                    FirebaseUser user = mAuth.getCurrentUser();
                    if (user != null) {
                        playerId = user.getUid();
                        signedIn = true;
                        playerName = "Player_" + playerId.substring(0, 6);
                        Log.i(TAG, "Anonymous sign-in success: " + playerId);

                        // Save player info
                        savePlayerInfo();
                    }
                } else {
                    Log.e(TAG, "Anonymous sign-in failed: " +
                        (task.getException() != null ? task.getException().getMessage() : "unknown"));
                    signedIn = false;
                }
            });
    }

    /**
     * Save player info to Firestore
     */
    private void savePlayerInfo() {
        if (!signedIn || playerId == null || db == null) return;

        Map<String, Object> player = new HashMap<>();
        player.put("name", playerName);
        player.put("createdAt", System.currentTimeMillis());
        player.put("totalGames", 0);
        player.put("bestScore", 0);
        player.put("totalCoins", 0);

        db.collection(COLLECTION_PLAYERS).document(playerId)
            .set(player)
            .addOnSuccessListener(aVoid -> Log.i(TAG, "Player info saved"))
            .addOnFailureListener(e -> Log.e(TAG, "Player info save failed: " + e.getMessage()));
    }

    /**
     * Submit score to Leaderboard
     * Called when game is over
     */
    public void submitScore(int score, int coins, ScoreCallback callback) {
        if (!signedIn || playerId == null || db == null) {
            if (callback != null) callback.onError("Not signed in");
            return;
        }

        Map<String, Object> entry = new HashMap<>();
        entry.put("playerId", playerId);
        entry.put("playerName", playerName != null ? playerName : "Unknown");
        entry.put("score", score);
        entry.put("coins", coins);
        entry.put("timestamp", System.currentTimeMillis());

        db.collection(COLLECTION_LEADERBOARD)
            .add(entry)
            .addOnSuccessListener(documentReference -> {
                Log.i(TAG, "Score submitted: " + score);
                // Also update player's best score
                updatePlayerBest(score, coins);
                if (callback != null) callback.onSuccess();
            })
            .addOnFailureListener(e -> {
                Log.e(TAG, "Score submit failed: " + e.getMessage());
                if (callback != null) callback.onError(e.getMessage());
            });
    }

    /**
     * Update player's best score and total stats
     */
    private void updatePlayerBest(int score, int coins) {
        if (!signedIn || playerId == null || db == null) return;

        db.collection(COLLECTION_PLAYERS).document(playerId)
            .get()
            .addOnSuccessListener(doc -> {
                if (doc.exists()) {
                    Long totalGames = doc.getLong("totalGames");
                    Long bestScore = doc.getLong("bestScore");
                    Long totalCoins = doc.getLong("totalCoins");

                    int newTotalGames = (totalGames != null ? totalGames.intValue() : 0) + 1;
                    int newBestScore = bestScore != null ? Math.max(bestScore.intValue(), score) : score;
                    int newTotalCoins = (totalCoins != null ? totalCoins.intValue() : 0) + coins;

                    Map<String, Object> updates = new HashMap<>();
                    updates.put("totalGames", newTotalGames);
                    updates.put("bestScore", newBestScore);
                    updates.put("totalCoins", newTotalCoins);
                    updates.put("lastPlayed", System.currentTimeMillis());

                    db.collection(COLLECTION_PLAYERS).document(playerId)
                        .update(updates)
                        .addOnSuccessListener(aVoid -> Log.i(TAG, "Player stats updated"))
                        .addOnFailureListener(e -> Log.e(TAG, "Player stats update failed"));
                }
            });
    }

    /**
     * Get top 20 leaderboard entries
     */
    public void getLeaderboard(LeaderboardCallback callback) {
        if (db == null) {
            if (callback != null) callback.onResult(new ArrayList<>());
            return;
        }

        db.collection(COLLECTION_LEADERBOARD)
            .orderBy("score", Query.Direction.DESCENDING)
            .limit(20)
            .get()
            .addOnCompleteListener(task -> {
                List<LeaderboardEntry> entries = new ArrayList<>();
                if (task.isSuccessful() && task.getResult() != null) {
                    for (QueryDocumentSnapshot doc : task.getResult()) {
                        String name = doc.getString("playerName");
                        Long scoreVal = doc.getLong("score");
                        Long coinsVal = doc.getLong("coins");
                        Long tsVal = doc.getLong("timestamp");

                        entries.add(new LeaderboardEntry(
                            name != null ? name : "Unknown",
                            scoreVal != null ? scoreVal.intValue() : 0,
                            coinsVal != null ? coinsVal.intValue() : 0,
                            tsVal != null ? tsVal : 0
                        ));
                    }
                } else {
                    Log.e(TAG, "Leaderboard fetch failed");
                }
                if (callback != null) callback.onResult(entries);
            });
    }

    /**
     * Get player's best score
     */
    public void getPlayerBestScore(ScoreCallback2 callback) {
        if (!signedIn || playerId == null || db == null) {
            if (callback != null) callback.onResult(0);
            return;
        }

        db.collection(COLLECTION_PLAYERS).document(playerId)
            .get()
            .addOnSuccessListener(doc -> {
                if (doc.exists()) {
                    Long bestScore = doc.getLong("bestScore");
                    if (callback != null) callback.onResult(bestScore != null ? bestScore.intValue() : 0);
                } else {
                    if (callback != null) callback.onResult(0);
                }
            })
            .addOnFailureListener(e -> {
                if (callback != null) callback.onResult(0);
            });
    }

    // Second callback interface for getting int result
    public interface ScoreCallback2 {
        void onResult(int bestScore);
    }

    // Getters
    public boolean isInitialized() { return initialized; }
    public boolean isSignedIn() { return signedIn; }
    public String getPlayerId() { return playerId; }
    public String getPlayerName() { return playerName; }
}
