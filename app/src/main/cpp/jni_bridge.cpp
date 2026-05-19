/*
 * ============================================
 *   JNI BRIDGE - 3D Racing Game
 *   Connects Java GLSurfaceView to C++ Engine
 * ============================================
 */

#include <jni.h>
#include <GLES2/gl2.h>
#include <android/log.h>
#include <chrono>
#include "game3d_engine.h"
#include "renderer3d.h"

#define LOG_TAG "RacingGame3D"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static Game3DEngine* gameEngine = nullptr;
static Renderer3D* renderer = nullptr;
static auto lastFrameTime = std::chrono::high_resolution_clock::now();

extern "C" {

// ====== Lifecycle ======

JNIEXPORT void JNICALL
Java_com_racing_game_GameRenderer_nativeInit(JNIEnv* env, jobject thiz) {
    LOGI("nativeInit called");
    if (gameEngine) { delete gameEngine; gameEngine = nullptr; }
    if (renderer) { delete renderer; renderer = nullptr; }

    gameEngine = new Game3DEngine();
    renderer = new Renderer3D();
    renderer->setEngine(gameEngine);
    renderer->init();
    lastFrameTime = std::chrono::high_resolution_clock::now();
    LOGI("nativeInit done");
}

JNIEXPORT void JNICALL
Java_com_racing_game_GameRenderer_nativeResize(JNIEnv* env, jobject thiz, jint width, jint height) {
    if (renderer) {
        renderer->resize(width, height);
    }
}

JNIEXPORT void JNICALL
Java_com_racing_game_GameRenderer_nativeRender(JNIEnv* env, jobject thiz) {
    if (!gameEngine || !renderer) return;

    // Calculate delta time
    auto now = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float>(now - lastFrameTime).count();
    lastFrameTime = now;
    if (dt > 0.1f) dt = 0.1f;

    gameEngine->update(dt);
    renderer->render();
}

JNIEXPORT void JNICALL
Java_com_racing_game_GameRenderer_nativeDestroy(JNIEnv* env, jobject thiz) {
    LOGI("nativeDestroy");
    if (renderer) { renderer->cleanup(); delete renderer; renderer = nullptr; }
    if (gameEngine) { delete gameEngine; gameEngine = nullptr; }
}

// ====== Input ======

JNIEXPORT void JNICALL
Java_com_racing_game_GameRenderer_nativeSteerLeft(JNIEnv* env, jobject thiz, jfloat dt) {
    if (gameEngine) gameEngine->steerLeft(dt);
}

JNIEXPORT void JNICALL
Java_com_racing_game_GameRenderer_nativeSteerRight(JNIEnv* env, jobject thiz, jfloat dt) {
    if (gameEngine) gameEngine->steerRight(dt);
}

JNIEXPORT void JNICALL
Java_com_racing_game_GameRenderer_nativeSetPlayerX(JNIEnv* env, jobject thiz, jfloat x) {
    if (gameEngine) gameEngine->setPlayerX(x);
}

JNIEXPORT void JNICALL
Java_com_racing_game_GameRenderer_nativeNitro(JNIEnv* env, jobject thiz) {
    if (gameEngine) gameEngine->nitro();
}

JNIEXPORT void JNICALL
Java_com_racing_game_GameRenderer_nativePause(JNIEnv* env, jobject thiz) {
    if (gameEngine) gameEngine->togglePause();
}

JNIEXPORT void JNICALL
Java_com_racing_game_GameRenderer_nativeRestart(JNIEnv* env, jobject thiz) {
    if (gameEngine) gameEngine->restart();
    if (renderer) renderer->initScenery();
}

// ====== State Getters ======

JNIEXPORT jint JNICALL
Java_com_racing_game_GameRenderer_nativeGetScore(JNIEnv* env, jobject thiz) {
    return gameEngine ? gameEngine->getState().score : 0;
}

JNIEXPORT jint JNICALL
Java_com_racing_game_GameRenderer_nativeGetHighScore(JNIEnv* env, jobject thiz) {
    return gameEngine ? gameEngine->getState().highScore : 0;
}

JNIEXPORT jint JNICALL
Java_com_racing_game_GameRenderer_nativeGetCoins(JNIEnv* env, jobject thiz) {
    return gameEngine ? gameEngine->getState().coins : 0;
}

JNIEXPORT jint JNICALL
Java_com_racing_game_GameRenderer_nativeGetLives(JNIEnv* env, jobject thiz) {
    return gameEngine ? gameEngine->getState().lives : 0;
}

JNIEXPORT jint JNICALL
Java_com_racing_game_GameRenderer_nativeGetSpeedKmh(JNIEnv* env, jobject thiz) {
    return gameEngine ? gameEngine->getSpeedKmh() : 0;
}

JNIEXPORT jboolean JNICALL
Java_com_racing_game_GameRenderer_nativeIsGameOver(JNIEnv* env, jobject thiz) {
    return gameEngine ? gameEngine->getState().gameOver : false;
}

JNIEXPORT jboolean JNICALL
Java_com_racing_game_GameRenderer_nativeIsPaused(JNIEnv* env, jobject thiz) {
    return gameEngine ? gameEngine->getState().paused : false;
}

JNIEXPORT jboolean JNICALL
Java_com_racing_game_GameRenderer_nativeHasShield(JNIEnv* env, jobject thiz) {
    return gameEngine ? gameEngine->getState().shield : false;
}

} // extern "C"
