/*
 * ============================================
 *   3D RACING GAME ENGINE
 *   Full 3D game logic with physics
 * ============================================
 */
#ifndef GAME3D_ENGINE_H
#define GAME3D_ENGINE_H

#include "math3d.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

enum ObjType { OBJ_CAR_SLOW, OBJ_CAR_FAST, OBJ_TRUCK, OBJ_COIN };

struct Obstacle3D {
    Vec3 pos;
    ObjType type;
    float speed;
    float laneX;  // target X position
    bool active;
    bool collected;
};

struct Game3DState {
    // Player
    Vec3 playerPos;
    float playerAngle;   // steering angle
    float playerSpeed;
    float targetSpeed;

    // Game state
    int score;
    int highScore;
    int coins;
    int lives;
    float gameSpeed;
    bool gameOver;
    bool paused;
    bool shield;
    float shieldTimer;
    float distance;
    float nitroTimer;

    // Obstacles
    std::vector<Obstacle3D> obstacles;
    float spawnTimer;

    // Scenery
    float lastTreeZ;
    float lastBuildingZ;
};

class Game3DEngine {
private:
    Game3DState state;
    float roadOffset;
    bool initialized;

    float getLaneX(int lane) {
        float lanes[] = {-4.0f, -1.5f, 1.5f, 4.0f};
        return lanes[lane % 4];
    }

public:
    Game3DEngine() : initialized(false) {
        srand((unsigned int)time(0));
        state.highScore = 0;
        reset();
    }

    void reset() {
        state.playerPos = Vec3(0, 0, 0);
        state.playerAngle = 0;
        state.playerSpeed = 30.0f;
        state.targetSpeed = 30.0f;
        state.score = 0;
        state.coins = 0;
        state.lives = 3;
        state.gameSpeed = 30.0f;
        state.gameOver = false;
        state.paused = false;
        state.shield = false;
        state.shieldTimer = 0;
        state.distance = 0;
        state.nitroTimer = 0;
        state.obstacles.clear();
        state.spawnTimer = 0;
        state.lastTreeZ = -20;
        state.lastBuildingZ = -30;
        roadOffset = 0;
        initialized = true;
    }

    void update(float dt) {
        if (state.gameOver || state.paused) return;

        // Increase speed over time
        state.gameSpeed += 1.5f * dt;
        if (state.gameSpeed > 100.0f) state.gameSpeed = 100.0f;

        // Nitro
        if (state.nitroTimer > 0) {
            state.nitroTimer -= dt;
            state.playerSpeed = state.gameSpeed + 40.0f;
            if (state.nitroTimer <= 0) {
                state.playerSpeed = state.gameSpeed;
            }
        } else {
            state.playerSpeed = state.gameSpeed;
        }

        // Shield
        if (state.shield) {
            state.shieldTimer -= dt;
            if (state.shieldTimer <= 0) state.shield = false;
        }

        // Distance
        state.distance += state.playerSpeed * dt;
        state.score = (int)(state.distance * 0.5f) + state.coins * 50;

        // Steering smoothing
        state.playerAngle *= 0.95f;

        // Keep player on road
        if (state.playerPos.x < -5.0f) { state.playerPos.x = -5.0f; state.playerAngle = 0; }
        if (state.playerPos.x > 5.0f) { state.playerPos.x = 5.0f; state.playerAngle = 0; }

        // Spawn obstacles
        float spawnInterval = 1.5f - state.gameSpeed * 0.008f;
        spawnInterval = std::max(0.5f, spawnInterval);
        state.spawnTimer += dt;
        if (state.spawnTimer >= spawnInterval) {
            spawnObstacle();
            state.spawnTimer = 0;
        }

        // Move obstacles (they appear to come toward player)
        for (auto& obs : state.obstacles) {
            if (!obs.active) continue;
            float relSpeed = state.playerSpeed - obs.speed;
            obs.pos.z += relSpeed * dt;
        }

        // Remove off-screen obstacles
        for (auto it = state.obstacles.begin(); it != state.obstacles.end(); ) {
            if (it->pos.z > 30.0f) {
                it = state.obstacles.erase(it);
            } else {
                it++;
            }
        }

        // Check coin collection
        for (auto it = state.obstacles.begin(); it != state.obstacles.end(); ) {
            if (it->type == OBJ_COIN && it->active && !it->collected) {
                Vec3 diff = state.playerPos - it->pos;
                if (diff.x*diff.x + diff.z*diff.z < 4.0f) {
                    state.coins++;
                    it->collected = true;
                    it->active = false;
                    it = state.obstacles.erase(it);
                    continue;
                }
            }
            it++;
        }

        // Collision detection
        for (auto& obs : state.obstacles) {
            if (!obs.active || obs.type == OBJ_COIN) continue;
            Vec3 diff = state.playerPos - obs.pos;
            float dx = fabsf(diff.x);
            float dz = fabsf(diff.z);
            float carW = (obs.type == OBJ_TRUCK) ? 2.5f : 2.0f;
            float carL = (obs.type == OBJ_TRUCK) ? 5.5f : 4.0f;
            if (dx < carW && dz < carL) {
                if (!state.shield) {
                    state.lives--;
                    if (state.lives <= 0) {
                        state.gameOver = true;
                        if (state.score > state.highScore) state.highScore = state.score;
                    } else {
                        state.shield = true;
                        state.shieldTimer = 2.0f;
                    }
                }
                obs.active = false;
                break;
            }
        }
    }

    void spawnObstacle() {
        Obstacle3D obs;
        int lane = rand() % 4;
        obs.laneX = getLaneX(lane);
        obs.pos = Vec3(obs.laneX, 0, -80.0f - (float)(rand() % 40));
        obs.active = true;
        obs.collected = false;

        int roll = rand() % 100;
        if (roll < 40) {
            obs.type = OBJ_CAR_SLOW;
            obs.speed = state.playerSpeed * 0.3f;
        } else if (roll < 65) {
            obs.type = OBJ_CAR_FAST;
            obs.speed = state.playerSpeed * 0.6f;
        } else if (roll < 85) {
            obs.type = OBJ_TRUCK;
            obs.speed = state.playerSpeed * 0.2f;
        } else {
            obs.type = OBJ_COIN;
            obs.speed = state.playerSpeed * 0.4f;
            obs.pos.y = 0;
        }

        state.obstacles.push_back(obs);
    }

    void steerLeft(float dt) {
        if (state.gameOver || state.paused) return;
        state.playerPos.x -= 8.0f * dt;
        state.playerAngle = -15.0f;
    }

    void steerRight(float dt) {
        if (state.gameOver || state.paused) return;
        state.playerPos.x += 8.0f * dt;
        state.playerAngle = 15.0f;
    }

    void setPlayerX(float x) {
        if (state.gameOver || state.paused) return;
        state.playerPos.x = x;
    }

    void nitro() {
        if (state.gameOver || state.paused || state.nitroTimer > 0) return;
        state.nitroTimer = 2.0f;
    }

    void togglePause() {
        if (!state.gameOver) state.paused = !state.paused;
    }

    void restart() { reset(); }

    const Game3DState& getState() const { return state; }
    int getSpeedKmh() const { return (int)(state.playerSpeed * 3 + 40); }
};

#endif // GAME3D_ENGINE_H
