/*
 * ============================================
 *   🏎️ JNI Bridge - C++ to Java
 *   Connects GameEngine with Android Java UI
 * ============================================
 */

#include <jni.h>
#include <string>
#include "game_engine.h"

// Global game engine instance
static GameEngine* engine = nullptr;

extern "C" {

// ====== Lifecycle ======

JNIEXPORT void JNICALL
Java_com_racing_game_GameView_nativeInit(JNIEnv* env, jobject thiz) {
    if (engine == nullptr) {
        engine = new GameEngine();
    } else {
        engine->restart();
    }
}

JNIEXPORT void JNICALL
Java_com_racing_game_GameView_nativeUpdate(JNIEnv* env, jobject thiz, jfloat delta_time) {
    if (engine != nullptr) {
        engine->update(delta_time);
    }
}

JNIEXPORT void JNICALL
Java_com_racing_game_GameView_nativeMoveLeft(JNIEnv* env, jobject thiz) {
    if (engine != nullptr) {
        engine->moveLeft();
    }
}

JNIEXPORT void JNICALL
Java_com_racing_game_GameView_nativeMoveRight(JNIEnv* env, jobject thiz) {
    if (engine != nullptr) {
        engine->moveRight();
    }
}

JNIEXPORT void JNICALL
Java_com_racing_game_GameView_nativeSetPlayerX(JNIEnv* env, jobject thiz, jfloat x) {
    if (engine != nullptr) {
        engine->setPlayerPosition(x);
    }
}

JNIEXPORT void JNICALL
Java_com_racing_game_GameView_nativeNitro(JNIEnv* env, jobject thiz) {
    if (engine != nullptr) {
        engine->activateNitro();
    }
}

JNIEXPORT void JNICALL
Java_com_racing_game_GameView_nativePause(JNIEnv* env, jobject thiz) {
    if (engine != nullptr) {
        engine->togglePause();
    }
}

JNIEXPORT void JNICALL
Java_com_racing_game_GameView_nativeRestart(JNIEnv* env, jobject thiz) {
    if (engine != nullptr) {
        engine->restart();
    }
}

JNIEXPORT void JNICALL
Java_com_racing_game_GameView_nativeDestroy(JNIEnv* env, jobject thiz) {
    if (engine != nullptr) {
        delete engine;
        engine = nullptr;
    }
}

// ====== State Getters ======

JNIEXPORT jfloat JNICALL
Java_com_racing_game_GameView_nativeGetPlayerX(JNIEnv* env, jobject thiz) {
    return engine ? engine->getState().playerX : 50.0f;
}

JNIEXPORT jfloat JNICALL
Java_com_racing_game_GameView_nativeGetPlayerY(JNIEnv* env, jobject thiz) {
    return engine ? engine->getState().playerY : 80.0f;
}

JNIEXPORT jint JNICALL
Java_com_racing_game_GameView_nativeGetScore(JNIEnv* env, jobject thiz) {
    return engine ? engine->getState().score : 0;
}

JNIEXPORT jint JNICALL
Java_com_racing_game_GameView_nativeGetHighScore(JNIEnv* env, jobject thiz) {
    return engine ? engine->getState().highScore : 0;
}

JNIEXPORT jint JNICALL
Java_com_racing_game_GameView_nativeGetCoins(JNIEnv* env, jobject thiz) {
    return engine ? engine->getState().coins : 0;
}

JNIEXPORT jint JNICALL
Java_com_racing_game_GameView_nativeGetLives(JNIEnv* env, jobject thiz) {
    return engine ? engine->getState().lives : 0;
}

JNIEXPORT jint JNICALL
Java_com_racing_game_GameView_nativeGetSpeedKmh(JNIEnv* env, jobject thiz) {
    return engine ? engine->getSpeedKmh() : 0;
}

JNIEXPORT jboolean JNICALL
Java_com_racing_game_GameView_nativeIsGameOver(JNIEnv* env, jobject thiz) {
    return engine ? engine->getState().gameOver : false;
}

JNIEXPORT jboolean JNICALL
Java_com_racing_game_GameView_nativeIsPaused(JNIEnv* env, jobject thiz) {
    return engine ? engine->getState().paused : false;
}

JNIEXPORT jboolean JNICALL
Java_com_racing_game_GameView_nativeHasShield(JNIEnv* env, jobject thiz) {
    return engine ? engine->getState().shield : false;
}

JNIEXPORT jfloat JNICALL
Java_com_racing_game_GameView_nativeGetRoadOffset(JNIEnv* env, jobject thiz) {
    return engine ? engine->getRoadOffset() : 0.0f;
}

// ====== Obstacle Data ======

JNIEXPORT jint JNICALL
Java_com_racing_game_GameView_nativeGetObstacleCount(JNIEnv* env, jobject thiz) {
    return engine ? (jint)engine->getState().obstacles.size() : 0;
}

JNIEXPORT jfloat JNICALL
Java_com_racing_game_GameView_nativeGetObstacleX(JNIEnv* env, jobject thiz, jint index) {
    if (engine && index >= 0 && index < (jint)engine->getState().obstacles.size()) {
        return engine->getState().obstacles[index].x;
    }
    return 0.0f;
}

JNIEXPORT jfloat JNICALL
Java_com_racing_game_GameView_nativeGetObstacleY(JNIEnv* env, jobject thiz, jint index) {
    if (engine && index >= 0 && index < (jint)engine->getState().obstacles.size()) {
        return engine->getState().obstacles[index].y;
    }
    return 0.0f;
}

JNIEXPORT jfloat JNICALL
Java_com_racing_game_GameView_nativeGetObstacleWidth(JNIEnv* env, jobject thiz, jint index) {
    if (engine && index >= 0 && index < (jint)engine->getState().obstacles.size()) {
        return engine->getState().obstacles[index].width;
    }
    return 0.0f;
}

JNIEXPORT jfloat JNICALL
Java_com_racing_game_GameView_nativeGetObstacleHeight(JNIEnv* env, jobject thiz, jint index) {
    if (engine && index >= 0 && index < (jint)engine->getState().obstacles.size()) {
        return engine->getState().obstacles[index].height;
    }
    return 0.0f;
}

JNIEXPORT jint JNICALL
Java_com_racing_game_GameView_nativeGetObstacleType(JNIEnv* env, jobject thiz, jint index) {
    if (engine && index >= 0 && index < (jint)engine->getState().obstacles.size()) {
        return (jint)engine->getState().obstacles[index].type;
    }
    return 0;
}

JNIEXPORT jboolean JNICALL
Java_com_racing_game_GameView_nativeIsObstacleActive(JNIEnv* env, jobject thiz, jint index) {
    if (engine && index >= 0 && index < (jint)engine->getState().obstacles.size()) {
        return engine->getState().obstacles[index].active;
    }
    return false;
}

} // extern "C"
