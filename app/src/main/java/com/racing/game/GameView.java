package com.racing.game;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class GameView extends SurfaceView implements SurfaceHolder.Callback {

    // ====== Native Methods (JNI) ======
    public native void nativeInit();
    public native void nativeUpdate(float deltaTime);
    public native void nativeMoveLeft();
    public native void nativeMoveRight();
    public native void nativeSetPlayerX(float x);
    public native void nativeNitro();
    public native void nativePause();
    public native void nativeRestart();
    public native void nativeDestroy();

    public native float nativeGetPlayerX();
    public native float nativeGetPlayerY();
    public native int nativeGetScore();
    public native int nativeGetHighScore();
    public native int nativeGetCoins();
    public native int nativeGetLives();
    public native int nativeGetSpeedKmh();
    public native boolean nativeIsGameOver();
    public native boolean nativeIsPaused();
    public native boolean nativeHasShield();
    public native float nativeGetRoadOffset();

    public native int nativeGetObstacleCount();
    public native float nativeGetObstacleX(int index);
    public native float nativeGetObstacleY(int index);
    public native float nativeGetObstacleWidth(int index);
    public native float nativeGetObstacleHeight(int index);
    public native int nativeGetObstacleType(int index);
    public native boolean nativeIsObstacleActive(int index);

    static {
        System.loadLibrary("racinggame");
    }

    // ====== Game Thread ======
    private GameThread gameThread;
    private boolean running = false;
    private long lastTime = 0;

    // ====== Touch ======
    private float touchStartX = 0;
    private float touchLastX = 0;
    private boolean touching = false;

    // ====== Screen Dimensions ======
    private int screenWidth = 0;
    private int screenHeight = 0;

    // ====== Virtual Game Space ======
    private static final float GAME_WIDTH = 100.0f;
    private static final float GAME_HEIGHT = 100.0f;
    private static final float ROAD_LEFT = 10.0f;
    private static final float ROAD_RIGHT = 90.0f;
    private static final float CAR_WIDTH = 6.0f;
    private static final float CAR_HEIGHT = 10.0f;

    // ====== Paints ======
    private Paint roadPaint;
    private Paint grassPaint;
    private Paint linePaint;
    private Paint dashPaint;
    private Paint playerPaint;
    private Paint playerShieldPaint;
    private Paint slowCarPaint;
    private Paint fastCarPaint;
    private Paint truckPaint;
    private Paint coinPaint;
    private Paint hudPaint;
    private Paint hudSmallPaint;
    private Paint gameOverPaint;
    private Paint buttonPaint;
    private Paint nitroPaint;
    private Paint explosionPaint;
    private Paint bgPaint;

    // ====== Scale Helper ======
    private float scaleX = 1.0f;
    private float scaleY = 1.0f;
    private float offsetX = 0.0f;
    private float offsetY = 0.0f;

    // ====== Explosion particles ======
    private float explosionX = -1, explosionY = -1;
    private float explosionTimer = 0;

    public GameView(Context context) {
        super(context);
        getHolder().addCallback(this);
        setFocusable(true);

        initPaints();
        nativeInit();
    }

    private void initPaints() {
        roadPaint = new Paint();
        roadPaint.setColor(Color.parseColor("#3a3a3a"));
        roadPaint.setStyle(Paint.Style.FILL);

        grassPaint = new Paint();
        grassPaint.setColor(Color.parseColor("#2d5a1e"));
        grassPaint.setStyle(Paint.Style.FILL);

        linePaint = new Paint();
        linePaint.setColor(Color.WHITE);
        linePaint.setStyle(Paint.Style.FILL);

        dashPaint = new Paint();
        dashPaint.setColor(Color.parseColor("#FFC107"));
        dashPaint.setStyle(Paint.Style.FILL);

        playerPaint = new Paint();
        playerPaint.setColor(Color.parseColor("#4CAF50"));
        playerPaint.setStyle(Paint.Style.FILL);

        playerShieldPaint = new Paint();
        playerShieldPaint.setColor(Color.parseColor("#00BCD4"));
        playerPaint.setStyle(Paint.Style.FILL);

        slowCarPaint = new Paint();
        slowCarPaint.setColor(Color.parseColor("#F44336"));
        slowCarPaint.setStyle(Paint.Style.FILL);

        fastCarPaint = new Paint();
        fastCarPaint.setColor(Color.parseColor("#9C27B0"));
        fastCarPaint.setStyle(Paint.Style.FILL);

        truckPaint = new Paint();
        truckPaint.setColor(Color.parseColor("#FF9800"));
        truckPaint.setStyle(Paint.Style.FILL);

        coinPaint = new Paint();
        coinPaint.setColor(Color.parseColor("#FFD700"));
        coinPaint.setStyle(Paint.Style.FILL);

        hudPaint = new Paint();
        hudPaint.setColor(Color.WHITE);
        hudPaint.setTextSize(40);
        hudPaint.setFakeBoldText(true);
        hudPaint.setAntiAlias(true);

        hudSmallPaint = new Paint();
        hudSmallPaint.setColor(Color.WHITE);
        hudSmallPaint.setTextSize(28);
        hudSmallPaint.setAntiAlias(true);

        gameOverPaint = new Paint();
        gameOverPaint.setColor(Color.RED);
        gameOverPaint.setTextSize(72);
        gameOverPaint.setFakeBoldText(true);
        gameOverPaint.setAntiAlias(true);

        buttonPaint = new Paint();
        buttonPaint.setColor(Color.parseColor("#B39DDB"));
        buttonPaint.setStyle(Paint.Style.FILL);
        buttonPaint.setAntiAlias(true);

        nitroPaint = new Paint();
        nitroPaint.setColor(Color.parseColor("#FF5722"));
        nitroPaint.setStyle(Paint.Style.FILL);
        nitroPaint.setAntiAlias(true);

        explosionPaint = new Paint();
        explosionPaint.setColor(Color.parseColor("#FF5722"));
        explosionPaint.setStyle(Paint.Style.FILL);

        bgPaint = new Paint();
        bgPaint.setColor(Color.parseColor("#1B5E20"));
        bgPaint.setStyle(Paint.Style.FILL);
    }

    private void updateScale() {
        if (screenWidth > 0 && screenHeight > 0) {
            // Fit game into screen with letterboxing
            float gameAspect = GAME_WIDTH / GAME_HEIGHT;
            float screenAspect = (float) screenWidth / screenHeight;

            if (screenAspect > gameAspect) {
                scaleY = screenHeight / GAME_HEIGHT;
                scaleX = scaleY;
                offsetX = (screenWidth - GAME_WIDTH * scaleX) / 2;
                offsetY = 0;
            } else {
                scaleX = screenWidth / GAME_WIDTH;
                scaleY = scaleX;
                offsetX = 0;
                offsetY = (screenHeight - GAME_HEIGHT * scaleY) / 2;
            }
        }
    }

    private float gx(float gameX) { return offsetX + gameX * scaleX; }
    private float gy(float gameY) { return offsetY + gameY * scaleY; }
    private float gs(float gameSize) { return gameSize * scaleX; }

    // ====== Rendering ======

    @Override
    public void draw(Canvas canvas) {
        super.draw(canvas);
        if (canvas == null) return;

        canvas.drawColor(Color.parseColor("#1a1a2e"));

        drawGrass(canvas);
        drawRoad(canvas);
        drawLaneMarkings(canvas);
        drawObstacles(canvas);
        drawPlayerCar(canvas);
        drawHUD(canvas);

        if (nativeIsGameOver()) {
            drawGameOver(canvas);
        } else if (nativeIsPaused()) {
            drawPaused(canvas);
        }
    }

    private void drawGrass(Canvas canvas) {
        // Left grass
        canvas.drawRect(
            gx(0), gy(0), gx(ROAD_LEFT), gy(GAME_HEIGHT),
            grassPaint
        );
        // Right grass
        canvas.drawRect(
            gx(ROAD_RIGHT), gy(0), gx(GAME_WIDTH), gy(GAME_HEIGHT),
            grassPaint
        );

        // Grass stripe details
        Paint stripePaint = new Paint();
        stripePaint.setColor(Color.parseColor("#3a7a2a"));
        stripePaint.setStyle(Paint.Style.FILL);
        float offset = (nativeGetRoadOffset() * scaleX) % (gs(4));
        for (float y = -gs(4) + offset; y < gy(GAME_HEIGHT); y += gs(4)) {
            canvas.drawRect(gx(0), y, gx(ROAD_LEFT), y + gs(2), stripePaint);
            canvas.drawRect(gx(ROAD_RIGHT), y, gx(GAME_WIDTH), y + gs(2), stripePaint);
        }
    }

    private void drawRoad(Canvas canvas) {
        canvas.drawRect(
            gx(ROAD_LEFT), gy(0), gx(ROAD_RIGHT), gy(GAME_HEIGHT),
            roadPaint
        );

        // Road border lines
        canvas.drawRect(gx(ROAD_LEFT), gy(0), gx(ROAD_LEFT + 0.8f), gy(GAME_HEIGHT), linePaint);
        canvas.drawRect(gx(ROAD_RIGHT - 0.8f), gy(0), gx(ROAD_RIGHT), gy(GAME_HEIGHT), linePaint);
    }

    private void drawLaneMarkings(Canvas canvas) {
        float laneWidth = (ROAD_RIGHT - ROAD_LEFT) / 4;
        float offset = (nativeGetRoadOffset() * scaleY) % (gs(5));

        for (int lane = 1; lane < 4; lane++) {
            float x = ROAD_LEFT + laneWidth * lane;
            for (float y = -gs(5) + offset; y < gy(GAME_HEIGHT); y += gs(5)) {
                canvas.drawRect(
                    gx(x - 0.3f), y,
                    gx(x + 0.3f), y + gs(3),
                    dashPaint
                );
            }
        }
    }

    private void drawPlayerCar(Canvas canvas) {
        float px = nativeGetPlayerX();
        float py = nativeGetPlayerY();
        boolean hasShield = nativeHasShield();

        Paint carPaint = hasShield ? playerShieldPaint : playerPaint;

        float left = gx(px - CAR_WIDTH / 2);
        float top = gy(py - CAR_HEIGHT / 2);
        float right = gx(px + CAR_WIDTH / 2);
        float bottom = gy(py + CAR_HEIGHT / 2);

        // Car shadow
        Paint shadowPaint = new Paint();
        shadowPaint.setColor(Color.parseColor("#1a1a1a"));
        shadowPaint.setAlpha(80);
        canvas.drawRoundRect(new RectF(left + gs(1), top + gs(1), right + gs(1), bottom + gs(1)),
            gs(1.5f), gs(1.5f), shadowPaint);

        // Car body
        canvas.drawRoundRect(new RectF(left, top, right, bottom),
            gs(1.5f), gs(1.5f), carPaint);

        // Car roof / cockpit
        Paint roofPaint = new Paint();
        roofPaint.setColor(hasShield ? Color.parseColor("#0097A7") : Color.parseColor("#388E3C"));
        roofPaint.setStyle(Paint.Style.FILL);
        canvas.drawRoundRect(
            new RectF(left + gs(0.8f), top + gs(2), right - gs(0.8f), bottom - gs(2)),
            gs(1), gs(1), roofPaint
        );

        // Windshield
        Paint windPaint = new Paint();
        windPaint.setColor(Color.parseColor("#81D4FA"));
        windPaint.setStyle(Paint.Style.FILL);
        canvas.drawRect(
            left + gs(1.2f), top + gs(2.5f),
            right - gs(1.2f), top + gs(5),
            windPaint
        );

        // Headlights
        Paint headPaint = new Paint();
        headPaint.setColor(Color.YELLOW);
        headPaint.setStyle(Paint.Style.FILL);
        canvas.drawCircle(left + gs(1), top + gs(0.5f), gs(0.5f), headPaint);
        canvas.drawCircle(right - gs(1), top + gs(0.5f), gs(0.5f), headPaint);

        // Shield glow effect
        if (hasShield) {
            Paint shieldPaint = new Paint();
            shieldPaint.setColor(Color.parseColor("#00BCD4"));
            shieldPaint.setAlpha(60);
            shieldPaint.setStyle(Paint.Style.STROKE);
            shieldPaint.setStrokeWidth(gs(1));
            canvas.drawRoundRect(new RectF(left - gs(1), top - gs(1), right + gs(1), bottom + gs(1)),
                gs(2), gs(2), shieldPaint);
        }

        // Exhaust flame when fast
        int speed = nativeGetSpeedKmh();
        if (speed > 120) {
            Paint flamePaint = new Paint();
            flamePaint.setColor(Color.parseColor("#FF5722"));
            flamePaint.setStyle(Paint.Style.FILL);
            float flameLen = gs(2 + (speed - 120) * 0.05f);
            canvas.drawRect(
                left + gs(1.5f), bottom,
                left + gs(2.5f), bottom + flameLen,
                flamePaint
            );
            canvas.drawRect(
                right - gs(2.5f), bottom,
                right - gs(1.5f), bottom + flameLen,
                flamePaint
            );
        }
    }

    private void drawObstacles(Canvas canvas) {
        int count = nativeGetObstacleCount();
        for (int i = 0; i < count; i++) {
            if (!nativeIsObstacleActive(i)) continue;

            float ox = nativeGetObstacleX(i);
            float oy = nativeGetObstacleY(i);
            float ow = nativeGetObstacleWidth(i);
            float oh = nativeGetObstacleHeight(i);
            int type = nativeGetObstacleType(i);

            float left = gx(ox - ow / 2);
            float top = gy(oy - oh / 2);
            float right = gx(ox + ow / 2);
            float bottom = gy(oy + oh / 2);

            switch (type) {
                case 0: // CAR_SLOW
                    canvas.drawRoundRect(new RectF(left, top, right, bottom),
                        gs(1.5f), gs(1.5f), slowCarPaint);
                    // Windows
                    Paint sw = new Paint();
                    sw.setColor(Color.parseColor("#E57373"));
                    canvas.drawRect(left + gs(0.8f), top + gs(1.5f), right - gs(0.8f), top + gs(4), sw);
                    break;

                case 1: // CAR_FAST
                    canvas.drawRoundRect(new RectF(left, top, right, bottom),
                        gs(1.5f), gs(1.5f), fastCarPaint);
                    Paint fw = new Paint();
                    fw.setColor(Color.parseColor("#CE93D8"));
                    canvas.drawRect(left + gs(0.8f), top + gs(1.5f), right - gs(0.8f), top + gs(4), fw);
                    break;

                case 2: // TRUCK
                    canvas.drawRoundRect(new RectF(left, top, right, bottom),
                        gs(1), gs(1), truckPaint);
                    // Truck cargo line
                    Paint tl = new Paint();
                    tl.setColor(Color.parseColor("#E65100"));
                    canvas.drawRect(left + gs(0.3f), top + gs(2), right - gs(0.3f), top + gs(2.5f), tl);
                    canvas.drawRect(left + gs(0.3f), top + gs(5), right - gs(0.3f), top + gs(5.5f), tl);
                    break;

                case 3: // COIN
                    Paint coinInner = new Paint();
                    coinInner.setColor(Color.parseColor("#FFA000"));
                    coinInner.setStyle(Paint.Style.FILL);
                    canvas.drawCircle(gx(ox), gy(oy), gs(ow / 2), coinPaint);
                    canvas.drawCircle(gx(ox), gy(oy), gs(ow / 3), coinInner);
                    // Dollar sign
                    Paint ds = new Paint();
                    ds.setColor(Color.parseColor("#5D4037"));
                    ds.setTextSize(gs(3));
                    ds.setFakeBoldText(true);
                    ds.setAntiAlias(true);
                    ds.setTextAlign(Paint.Align.CENTER);
                    canvas.drawText("$", gx(ox), gy(oy) + gs(1), ds);
                    break;
            }
        }
    }

    private void drawHUD(Canvas canvas) {
        int score = nativeGetScore();
        int coins = nativeGetCoins();
        int lives = nativeGetLives();
        int speed = nativeGetSpeedKmh();
        int highScore = nativeGetHighScore();
        boolean hasShield = nativeHasShield();

        // HUD background
        Paint hudBg = new Paint();
        hudBg.setColor(Color.parseColor("#1a1a2e"));
        hudBg.setAlpha(180);
        hudBg.setStyle(Paint.Style.FILL);
        canvas.drawRect(0, 0, screenWidth, gy(12), hudBg);

        // Score
        hudPaint.setColor(Color.WHITE);
        canvas.drawText("Score: " + score, gx(1), gy(8), hudPaint);

        // Coins
        hudPaint.setColor(Color.parseColor("#FFD700"));
        canvas.drawText("Coins: $" + coins, gx(25), gy(8), hudPaint);

        // Lives (hearts)
        hudPaint.setColor(Color.RED);
        StringBuilder hearts = new StringBuilder();
        for (int i = 0; i < lives; i++) hearts.append("\u2665 ");
        canvas.drawText("Lives: " + hearts.toString(), gx(48), gy(8), hudPaint);

        // Speed
        hudPaint.setColor(Color.parseColor("#4CAF50"));
        canvas.drawText(speed + " km/h", gx(78), gy(8), hudPaint);

        // High score (small)
        hudSmallPaint.setColor(Color.parseColor("#CE93D8"));
        canvas.drawText("Best: " + highScore, gx(1), gy(11.5f), hudSmallPaint);

        // Shield indicator
        if (hasShield) {
            hudSmallPaint.setColor(Color.parseColor("#00BCD4"));
            canvas.drawText("[SHIELD]", gx(25), gy(11.5f), hudSmallPaint);
        }

        // Nitro button (bottom right)
        float nbX = gx(85);
        float nbY = gy(88);
        float nbR = gs(4);
        canvas.drawCircle(nbX, nbY, nbR, nitroPaint);
        Paint nitroText = new Paint();
        nitroText.setColor(Color.WHITE);
        nitroText.setTextSize(gs(2.5f));
        nitroText.setFakeBoldText(true);
        nitroText.setAntiAlias(true);
        nitroText.setTextAlign(Paint.Align.CENTER);
        canvas.drawText("N2O", nbX, nbY + gs(1), nitroText);

        // Pause button (top right)
        float pbX = gx(95);
        float pbY = gy(6);
        float pbR = gs(3);
        canvas.drawCircle(pbX, pbY, pbR, buttonPaint);
        Paint pauseText = new Paint();
        pauseText.setColor(Color.WHITE);
        pauseText.setTextSize(gs(2.5f));
        pauseText.setFakeBoldText(true);
        pauseText.setAntiAlias(true);
        pauseText.setTextAlign(Paint.Align.CENTER);
        canvas.drawText("||", pbX, pbY + gs(1), pauseText);
    }

    private void drawGameOver(Canvas canvas) {
        // Dark overlay
        Paint overlay = new Paint();
        overlay.setColor(Color.BLACK);
        overlay.setAlpha(180);
        overlay.setStyle(Paint.Style.FILL);
        canvas.drawRect(0, 0, screenWidth, screenHeight, overlay);

        // Game Over text
        gameOverPaint.setColor(Color.RED);
        gameOverPaint.setTextAlign(Paint.Align.CENTER);
        canvas.drawText("GAME OVER", screenWidth / 2f, screenHeight / 2f - gs(8), gameOverPaint);

        // Score
        Paint scorePaint = new Paint();
        scorePaint.setColor(Color.YELLOW);
        scorePaint.setTextSize(48);
        scorePaint.setFakeBoldText(true);
        scorePaint.setAntiAlias(true);
        scorePaint.setTextAlign(Paint.Align.CENTER);
        canvas.drawText("Score: " + nativeGetScore(), screenWidth / 2f, screenHeight / 2f, scorePaint);

        canvas.drawText("Coins: $" + nativeGetCoins(), screenWidth / 2f, screenHeight / 2f + gs(6), scorePaint);

        // High score
        scorePaint.setColor(Color.parseColor("#CE93D8"));
        scorePaint.setTextSize(36);
        canvas.drawText("Best: " + nativeGetHighScore(), screenWidth / 2f, screenHeight / 2f + gs(12), scorePaint);

        // Restart button
        Paint restartPaint = new Paint();
        restartPaint.setColor(Color.parseColor("#4CAF50"));
        restartPaint.setStyle(Paint.Style.FILL);
        restartPaint.setAntiAlias(true);
        float rbW = gs(30);
        float rbH = gs(8);
        float rbX = screenWidth / 2f - rbW / 2;
        float rbY = screenHeight / 2f + gs(16);
        canvas.drawRoundRect(new RectF(rbX, rbY, rbX + rbW, rbY + rbH), gs(2), gs(2), restartPaint);

        Paint restartText = new Paint();
        restartText.setColor(Color.WHITE);
        restartText.setTextSize(36);
        restartText.setFakeBoldText(true);
        restartText.setAntiAlias(true);
        restartText.setTextAlign(Paint.Align.CENTER);
        canvas.drawText("RESTART", screenWidth / 2f, rbY + gs(5.5f), restartText);
    }

    private void drawPaused(Canvas canvas) {
        Paint overlay = new Paint();
        overlay.setColor(Color.BLACK);
        overlay.setAlpha(120);
        overlay.setStyle(Paint.Style.FILL);
        canvas.drawRect(0, 0, screenWidth, screenHeight, overlay);

        Paint pausePaint = new Paint();
        pausePaint.setColor(Color.YELLOW);
        pausePaint.setTextSize(64);
        pausePaint.setFakeBoldText(true);
        pausePaint.setAntiAlias(true);
        pausePaint.setTextAlign(Paint.Align.CENTER);
        canvas.drawText("PAUSED", screenWidth / 2f, screenHeight / 2f, pausePaint);

        Paint hintPaint = new Paint();
        hintPaint.setColor(Color.WHITE);
        hintPaint.setTextSize(28);
        hintPaint.setAntiAlias(true);
        hintPaint.setTextAlign(Paint.Align.CENTER);
        canvas.drawText("Tap to Resume", screenWidth / 2f, screenHeight / 2f + gs(6), hintPaint);
    }

    // ====== Touch Input ======

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        float x = event.getX();
        float y = event.getY();

        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                touchStartX = x;
                touchLastX = x;
                touching = true;

                // Game Over - check restart button
                if (nativeIsGameOver()) {
                    float rbW = gs(30);
                    float rbH = gs(8);
                    float rbX = screenWidth / 2f - rbW / 2;
                    float rbY = screenHeight / 2f + gs(16);
                    if (x >= rbX && x <= rbX + rbW && y >= rbY && y <= rbY + rbH) {
                        nativeRestart();
                    }
                    return true;
                }

                // Pause button
                float pbX = gx(95);
                float pbY = gy(6);
                float pbR = gs(3);
                if (Math.sqrt((x - pbX) * (x - pbX) + (y - pbY) * (y - pbY)) < pbR) {
                    nativePause();
                    return true;
                }

                // Nitro button
                float nbX = gx(85);
                float nbY = gy(88);
                float nbR = gs(4);
                if (Math.sqrt((x - nbX) * (x - nbX) + (y - nbY) * (y - nbY)) < nbR) {
                    nativeNitro();
                    return true;
                }

                // Paused - tap to resume
                if (nativeIsPaused()) {
                    nativePause();
                    return true;
                }
                break;

            case MotionEvent.ACTION_MOVE:
                if (touching && !nativeIsGameOver() && !nativeIsPaused()) {
                    float dx = x - touchLastX;
                    if (Math.abs(dx) > 5) {
                        // Convert screen position to game X coordinate
                        float gameX = (x - offsetX) / scaleX;
                        nativeSetPlayerX(gameX);
                        touchLastX = x;
                    }
                }
                break;

            case MotionEvent.ACTION_UP:
                touching = false;
                break;
        }

        return true;
    }

    // ====== Game Thread ======

    private class GameThread extends Thread {
        @Override
        public void run() {
            lastTime = System.nanoTime();
            while (running) {
                long now = System.nanoTime();
                float deltaTime = (now - lastTime) / 1_000_000_000.0f;
                lastTime = now;

                // Cap delta time to avoid spiral of death
                if (deltaTime > 0.1f) deltaTime = 0.1f;

                nativeUpdate(deltaTime);

                Canvas canvas = null;
                try {
                    canvas = getHolder().lockCanvas();
                    if (canvas != null) {
                        synchronized (getHolder()) {
                            draw(canvas);
                        }
                    }
                } finally {
                    if (canvas != null) {
                        getHolder().unlockCanvasAndPost(canvas);
                    }
                }

                // ~60 FPS target
                try {
                    Thread.sleep(16);
                } catch (InterruptedException e) {
                    break;
                }
            }
        }
    }

    // ====== Surface Callbacks ======

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        screenWidth = getWidth();
        screenHeight = getHeight();
        updateScale();
        startGame();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        screenWidth = width;
        screenHeight = height;
        updateScale();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        stopGame();
    }

    private void startGame() {
        running = true;
        gameThread = new GameThread();
        gameThread.start();
    }

    private void stopGame() {
        running = false;
        if (gameThread != null) {
            try {
                gameThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            gameThread = null;
        }
    }

    public void onPause() {
        stopGame();
        if (!nativeIsGameOver() && !nativeIsPaused()) {
            nativePause();
        }
    }

    public void onResume() {
        if (!running) {
            startGame();
        }
    }

    public void onDestroy() {
        stopGame();
        nativeDestroy();
    }
}
