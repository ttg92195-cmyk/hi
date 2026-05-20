package com.racing.game;

import android.content.Context;
import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.SoundPool;
import android.media.ToneGenerator;
import android.util.Log;

/**
 * Sound Manager - Handles all game audio
 * - Engine sound: Continuous sine wave with pitch varying by speed (AudioTrack)
 * - Coin pickup: Short high-pitched tone
 * - Crash/hit: Low rumble tone
 * - Nitro: Whoosh effect
 * - Background music: Simple looping tone
 */
public class SoundManager {

    private static final String TAG = "SoundManager";

    // Sound state
    private boolean soundEnabled = true;
    private boolean initialized = false;

    // AudioTrack for engine sound
    private AudioTrack engineTrack;
    private boolean enginePlaying = false;
    private float engineFrequency = 100f; // Hz, varies with speed
    private Thread engineThread;
    private volatile boolean engineRunning = false;

    // ToneGenerator for effects
    private ToneGenerator toneGen;

    // Context
    private Context context;

    // Volume
    private float masterVolume = 0.5f;

    public SoundManager(Context context) {
        this.context = context;
    }

    /**
     * Initialize audio systems
     */
    public void init() {
        try {
            toneGen = new ToneGenerator(AudioManager.STREAM_MUSIC, 50);
            initialized = true;
            Log.i(TAG, "SoundManager initialized");
        } catch (Exception e) {
            Log.e(TAG, "SoundManager init failed: " + e.getMessage());
            initialized = false;
        }
    }

    /**
     * Start engine sound loop
     */
    public void startEngine() {
        if (!soundEnabled || !initialized || engineRunning) return;
        engineRunning = true;

        engineThread = new Thread(() -> {
            try {
                int sampleRate = 22050;
                int bufferSize = AudioTrack.getMinBufferSize(
                    sampleRate,
                    AudioFormat.CHANNEL_OUT_MONO,
                    AudioFormat.ENCODING_PCM_16BIT
                );

                engineTrack = new AudioTrack.Builder()
                    .setAudioAttributes(new AudioAttributes.Builder()
                        .setUsage(AudioAttributes.USAGE_GAME)
                        .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION)
                        .build())
                    .setAudioFormat(new AudioFormat.Builder()
                        .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                        .setSampleRate(sampleRate)
                        .setChannelMask(AudioFormat.CHANNEL_OUT_MONO)
                        .build())
                    .setBufferSizeInBytes(bufferSize)
                    .setTransferMode(AudioTrack.MODE_STREAM)
                    .build();

                engineTrack.play();
                enginePlaying = true;

                short[] buffer = new short[bufferSize / 2];
                double phase = 0;

                while (engineRunning) {
                    double freq = engineFrequency;
                    double inc = 2.0 * Math.PI * freq / sampleRate;

                    for (int i = 0; i < buffer.length; i++) {
                        // Mix fundamental + harmonics for richer engine sound
                        double sample = 0.5 * Math.sin(phase);
                        sample += 0.25 * Math.sin(phase * 2.0); // 2nd harmonic
                        sample += 0.125 * Math.sin(phase * 3.0); // 3rd harmonic

                        // Add some noise for realism
                        sample += 0.05 * (Math.random() * 2.0 - 1.0);

                        buffer[i] = (short)(sample * 6000 * masterVolume);
                        phase += inc;
                        if (phase > 2.0 * Math.PI) phase -= 2.0 * Math.PI;
                    }

                    int result = engineTrack.write(buffer, 0, buffer.length);
                    if (result < 0) break;
                }

                engineTrack.stop();
                engineTrack.release();
                engineTrack = null;
                enginePlaying = false;

            } catch (Exception e) {
                Log.e(TAG, "Engine sound error: " + e.getMessage());
                enginePlaying = false;
                engineRunning = false;
            }
        });
        engineThread.setPriority(Thread.MIN_PRIORITY);
        engineThread.start();
    }

    /**
     * Stop engine sound
     */
    public void stopEngine() {
        engineRunning = false;
        if (engineThread != null) {
            try {
                engineThread.join(500);
            } catch (InterruptedException e) { /* ignore */ }
            engineThread = null;
        }
    }

    /**
     * Update engine frequency based on speed
     * @param speedKmh Current speed in km/h
     */
    public void updateEngineSpeed(int speedKmh) {
        // Map speed (0-250) to frequency (80-400 Hz)
        engineFrequency = 80f + (speedKmh / 250f) * 320f;
    }

    /**
     * Play coin pickup sound - short pleasant tone
     */
    public void playCoinSound() {
        if (!soundEnabled || !initialized) return;
        try {
            // High-pitched pleasant double-beep
            new Thread(() -> {
                try {
                    ToneGenerator t = new ToneGenerator(AudioManager.STREAM_MUSIC, 40);
                    t.startTone(ToneGenerator.TONE_CDMA_KEYPAD_VOLUME_KEY_LITE, 80);
                    try { Thread.sleep(80); } catch (Exception e) {}
                    t.stopTone();
                    try { Thread.sleep(40); } catch (Exception e) {}
                    t.startTone(ToneGenerator.TONE_CDMA_NETWORK_BUSY_ONE_SHOT, 100);
                    try { Thread.sleep(100); } catch (Exception e) {}
                    t.stopTone();
                    t.release();
                } catch (Exception e) {}
            }).start();
        } catch (Exception e) {
            Log.e(TAG, "Coin sound error: " + e.getMessage());
        }
    }

    /**
     * Play crash/hit sound - low rumble
     */
    public void playCrashSound() {
        if (!soundEnabled || !initialized) return;
        try {
            new Thread(() -> {
                try {
                    ToneGenerator t = new ToneGenerator(AudioManager.STREAM_MUSIC, 60);
                    t.startTone(ToneGenerator.TONE_CDMA_ABBR_ALERT, 300);
                    try { Thread.sleep(300); } catch (Exception e) {}
                    t.stopTone();
                    t.release();
                } catch (Exception e) {}
            }).start();
        } catch (Exception e) {
            Log.e(TAG, "Crash sound error: " + e.getMessage());
        }
    }

    /**
     * Play nitro boost sound - whoosh
     */
    public void playNitroSound() {
        if (!soundEnabled || !initialized) return;
        try {
            new Thread(() -> {
                try {
                    ToneGenerator t = new ToneGenerator(AudioManager.STREAM_MUSIC, 50);
                    // Rising pitch effect
                    t.startTone(ToneGenerator.TONE_CDMA_NETWORK_BUSY, 150);
                    try { Thread.sleep(150); } catch (Exception e) {}
                    t.stopTone();
                    t.startTone(ToneGenerator.TONE_CDMA_PIP, 200);
                    try { Thread.sleep(200); } catch (Exception e) {}
                    t.stopTone();
                    t.release();
                } catch (Exception e) {}
            }).start();
        } catch (Exception e) {
            Log.e(TAG, "Nitro sound error: " + e.getMessage());
        }
    }

    /**
     * Play game over sound
     */
    public void playGameOverSound() {
        if (!soundEnabled || !initialized) return;
        try {
            new Thread(() -> {
                try {
                    ToneGenerator t = new ToneGenerator(AudioManager.STREAM_MUSIC, 50);
                    t.startTone(ToneGenerator.TONE_CDMA_CALL_SIGNAL_ISDN_NORMAL, 500);
                    try { Thread.sleep(500); } catch (Exception e) {}
                    t.stopTone();
                    try { Thread.sleep(200); } catch (Exception e) {}
                    t.startTone(ToneGenerator.TONE_CDMA_SIGNAL_OFF, 800);
                    try { Thread.sleep(800); } catch (Exception e) {}
                    t.stopTone();
                    t.release();
                } catch (Exception e) {}
            }).start();
        } catch (Exception e) {
            Log.e(TAG, "Game over sound error: " + e.getMessage());
        }
    }

    /**
     * Play button click sound
     */
    public void playClickSound() {
        if (!soundEnabled || !initialized) return;
        try {
            new Thread(() -> {
                try {
                    ToneGenerator t = new ToneGenerator(AudioManager.STREAM_MUSIC, 20);
                    t.startTone(ToneGenerator.TONE_PROP_BEEP, 50);
                    try { Thread.sleep(50); } catch (Exception e) {}
                    t.stopTone();
                    t.release();
                } catch (Exception e) {}
            }).start();
        } catch (Exception e) {}
    }

    // Getters/Setters
    public boolean isSoundEnabled() { return soundEnabled; }
    public void setSoundEnabled(boolean enabled) {
        this.soundEnabled = enabled;
        if (!enabled) stopEngine();
    }
    public float getMasterVolume() { return masterVolume; }
    public void setMasterVolume(float vol) {
        this.masterVolume = Math.max(0f, Math.min(1f, vol));
    }

    /**
     * Release all audio resources
     */
    public void release() {
        stopEngine();
        if (toneGen != null) {
            toneGen.release();
            toneGen = null;
        }
        initialized = false;
    }
}
