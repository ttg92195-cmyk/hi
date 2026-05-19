/*
 * ============================================
 *   🏎️ RACING GAME ENGINE - C++ (Android)
 *   Pure game logic, no rendering code
 *   Called via JNI from Java
 * ============================================
 */

#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// ============== GAME CONFIG ==============
const float ROAD_LEFT = 10.0f;
const float ROAD_RIGHT = 90.0f;
const float ROAD_WIDTH = 80.0f;
const float CAR_WIDTH = 6.0f;
const float CAR_HEIGHT = 10.0f;
const float INITIAL_GAME_SPEED = 30.0f;  // units per second
const float MAX_GAME_SPEED = 80.0f;
const float SPEED_INCREASE_RATE = 0.5f;  // per second
const int INITIAL_LIVES = 3;

// ============== DATA STRUCTURES ==============

enum ObstacleType { CAR_SLOW, CAR_FAST, TRUCK, COIN };

struct GameObject {
    float x, y;           // center position (0-100 space)
    float width, height;
    ObstacleType type;
    float speed;          // units per second
    bool active;
};

struct GameState {
    float playerX, playerY;
    int score;
    int highScore;
    int coins;
    int lives;
    float gameSpeed;
    bool gameOver;
    bool paused;
    bool shield;
    float shieldTimer;
    std::vector<GameObject> obstacles;
};

// ============== GAME ENGINE CLASS ==============
class GameEngine {
private:
    GameState state;
    float spawnTimer;
    float roadOffset;
    int frameCount;
    float targetSpeed;
    float nitroTimer;

    void spawnObstacle() {
        GameObject obs;
        obs.active = true;

        // Random lane position
        float laneCount = 4.0f;
        float laneWidth = ROAD_WIDTH / laneCount;
        int lane = rand() % (int)laneCount;
        obs.x = ROAD_LEFT + laneWidth * (lane + 0.5f);

        int typeRoll = rand() % 100;
        if (typeRoll < 45) {
            obs.type = CAR_SLOW;
            obs.width = 5.0f;
            obs.height = 8.0f;
            obs.speed = state.gameSpeed * 0.3f;
        } else if (typeRoll < 70) {
            obs.type = CAR_FAST;
            obs.width = 5.0f;
            obs.height = 8.0f;
            obs.speed = state.gameSpeed * 0.6f;
        } else if (typeRoll < 85) {
            obs.type = TRUCK;
            obs.width = 7.0f;
            obs.height = 14.0f;
            obs.speed = state.gameSpeed * 0.2f;
        } else {
            obs.type = COIN;
            obs.width = 4.0f;
            obs.height = 4.0f;
            obs.speed = state.gameSpeed * 0.4f;
        }

        obs.y = -10.0f;  // spawn above screen
        state.obstacles.push_back(obs);
    }

    bool checkCollision(const GameObject& obs) {
        if (obs.type == COIN) return false;
        if (!obs.active) return false;

        float pLeft = state.playerX - CAR_WIDTH / 2;
        float pRight = state.playerX + CAR_WIDTH / 2;
        float pTop = state.playerY - CAR_HEIGHT / 2;
        float pBottom = state.playerY + CAR_HEIGHT / 2;

        float oLeft = obs.x - obs.width / 2;
        float oRight = obs.x + obs.width / 2;
        float oTop = obs.y - obs.height / 2;
        float oBottom = obs.y + obs.height / 2;

        return (pRight > oLeft && pLeft < oRight &&
                pBottom > oTop && pTop < oBottom);
    }

    bool checkCoinCollect(const GameObject& coin) {
        if (coin.type != COIN || !coin.active) return false;

        float dx = state.playerX - coin.x;
        float dy = state.playerY - coin.y;
        float dist = dx * dx + dy * dy;
        return dist < 100.0f;  // collection radius
    }

public:
    GameEngine() {
        srand((unsigned int)time(0));
        state.highScore = 0;
        reset();
    }

    void reset() {
        state.playerX = 50.0f;
        state.playerY = 80.0f;
        state.score = 0;
        state.coins = 0;
        state.gameSpeed = INITIAL_GAME_SPEED;
        state.lives = INITIAL_LIVES;
        state.gameOver = false;
        state.paused = false;
        state.shield = false;
        state.shieldTimer = 0;
        state.obstacles.clear();
        spawnTimer = 0;
        roadOffset = 0;
        frameCount = 0;
        targetSpeed = INITIAL_GAME_SPEED;
        nitroTimer = 0;
    }

    void update(float deltaTime) {
        if (state.gameOver || state.paused) return;

        frameCount++;
        roadOffset += state.gameSpeed * deltaTime;

        // Increase game speed over time
        if (state.gameSpeed < MAX_GAME_SPEED) {
            state.gameSpeed += SPEED_INCREASE_RATE * deltaTime;
        }

        // Nitro timer
        if (nitroTimer > 0) {
            nitroTimer -= deltaTime;
            if (nitroTimer <= 0) {
                targetSpeed = state.gameSpeed;
            }
        }

        // Shield timer
        if (state.shield) {
            state.shieldTimer -= deltaTime;
            if (state.shieldTimer <= 0) {
                state.shield = false;
            }
        }

        // Spawn obstacles
        float spawnInterval = 0.8f - (state.gameSpeed - INITIAL_GAME_SPEED) * 0.005f;
        spawnInterval = std::max(0.3f, spawnInterval);
        spawnTimer += deltaTime;
        if (spawnTimer >= spawnInterval) {
            spawnObstacle();
            spawnTimer = 0;
        }

        // Move obstacles
        for (auto& obs : state.obstacles) {
            obs.y += (state.gameSpeed - obs.speed) * deltaTime;
        }

        // Remove off-screen obstacles and add score
        for (auto it = state.obstacles.begin(); it != state.obstacles.end(); ) {
            if (it->y > 110.0f) {
                if (it->active && it->type != COIN) {
                    state.score += 10;
                }
                it = state.obstacles.erase(it);
            } else {
                it++;
            }
        }

        // Check coin collection
        for (auto it = state.obstacles.begin(); it != state.obstacles.end(); ) {
            if (it->type == COIN && it->active && checkCoinCollect(*it)) {
                state.coins++;
                state.score += 50;
                it = state.obstacles.erase(it);
            } else {
                it++;
            }
        }

        // Check collision
        for (auto& obs : state.obstacles) {
            if (obs.active && checkCollision(obs)) {
                if (!state.shield) {
                    state.lives--;
                    if (state.lives <= 0) {
                        state.gameOver = true;
                        if (state.score > state.highScore) {
                            state.highScore = state.score;
                        }
                    } else {
                        state.shield = true;
                        state.shieldTimer = 2.0f;
                    }
                }
                obs.active = false;
                break;
            }
        }

        // Score increases over time
        state.score += (int)(state.gameSpeed * deltaTime * 0.5f);
    }

    void moveLeft() {
        if (!state.gameOver && !state.paused) {
            state.playerX -= 3.0f;
            if (state.playerX - CAR_WIDTH / 2 < ROAD_LEFT + 1) {
                state.playerX = ROAD_LEFT + 1 + CAR_WIDTH / 2;
            }
        }
    }

    void moveRight() {
        if (!state.gameOver && !state.paused) {
            state.playerX += 3.0f;
            if (state.playerX + CAR_WIDTH / 2 > ROAD_RIGHT - 1) {
                state.playerX = ROAD_RIGHT - 1 - CAR_WIDTH / 2;
            }
        }
    }

    void setPlayerPosition(float x) {
        if (!state.gameOver && !state.paused) {
            state.playerX = x;
            if (state.playerX - CAR_WIDTH / 2 < ROAD_LEFT + 1) {
                state.playerX = ROAD_LEFT + 1 + CAR_WIDTH / 2;
            }
            if (state.playerX + CAR_WIDTH / 2 > ROAD_RIGHT - 1) {
                state.playerX = ROAD_RIGHT - 1 - CAR_WIDTH / 2;
            }
        }
    }

    void activateNitro() {
        if (!state.gameOver && !state.paused && nitroTimer <= 0) {
            targetSpeed = state.gameSpeed;
            state.gameSpeed += 30.0f;
            nitroTimer = 1.5f;
        }
    }

    void togglePause() {
        if (!state.gameOver) {
            state.paused = !state.paused;
        }
    }

    void restart() {
        reset();
    }

    // ============== GETTERS ==============
    const GameState& getState() const { return state; }
    float getRoadOffset() const { return roadOffset; }
    int getSpeedKmh() const { return (int)(state.gameSpeed * 3 + 40); }
};

#endif // GAME_ENGINE_H
