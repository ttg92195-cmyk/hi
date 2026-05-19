/*
 * ============================================
 *   3D RENDERER - OpenGL ES 2.0
 *   Full 3D rendering with lighting
 * ============================================
 */
#ifndef RENDERER3D_H
#define RENDERER3D_H

#include "math3d.h"
#include "shaders.h"
#include "mesh.h"
#include "game3d_engine.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cstdio>
#include <cstdlib>

class Renderer3D {
private:
    // Shader programs
    GLuint mainProgram;
    GLuint skyProgram;

    // Attribute locations
    GLint aPosition, aNormal, aColor;
    GLint skyAPosition;

    // Uniform locations
    GLint uMVP, uModel, uLightDir, uAmbientColor;
    GLint uFogColor, uFogNear, uFogFar;
    GLint skyUTopColor, skyUBottomColor;

    // Mesh VBOs
    GLuint playerCarVBO, playerCarIBO;
    int playerCarIndexCount;

    GLuint obstacleCarVBO, obstacleCarIBO;
    int obstacleCarIndexCount;

    GLuint truckVBO, truckIBO;
    int truckIndexCount;

    GLuint coinVBO, coinIBO;
    int coinIndexCount;

    GLuint roadVBO, roadIBO;
    int roadIndexCount;

    GLuint treeVBO, treeIBO;
    int treeIndexCount;

    GLuint buildingVBO, buildingIBO;
    int buildingIndexCount;

    GLuint groundVBO, groundIBO;
    int groundIndexCount;

    GLuint skyVBO;

    // Screen
    int screenWidth, screenHeight;
    float aspect;

    // Camera
    Vec3 cameraPos;
    Vec3 cameraTarget;

    // Scenery tracking
    float lastTreeSpawnZ;
    float lastBuildingSpawnZ;
    std::vector<Vec3> treePositions;
    std::vector<Vec3> buildingPositions;
    float buildingHeights[20];

    Game3DEngine* engine;

    GLuint compileShader(GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint len;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
            char* log = new char[len+1];
            glGetShaderInfoLog(shader, len, NULL, log);
            printf("Shader compile error: %s\n", log);
            delete[] log;
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }

    GLuint createProgram(const char* vs, const char* fs) {
        GLuint v = compileShader(GL_VERTEX_SHADER, vs);
        GLuint f = compileShader(GL_FRAGMENT_SHADER, fs);
        if (!v || !f) return 0;
        GLuint p = glCreateProgram();
        glAttachShader(p, v);
        glAttachShader(p, f);
        glLinkProgram(p);
        GLint linked;
        glGetProgramiv(p, GL_LINK_STATUS, &linked);
        if (!linked) {
            GLint len;
            glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
            char* log = new char[len+1];
            glGetProgramInfoLog(p, len, NULL, log);
            printf("Program link error: %s\n", log);
            delete[] log;
            return 0;
        }
        glDeleteShader(v);
        glDeleteShader(f);
        return p;
    }

    void uploadMesh(const Mesh& mesh, GLuint& vbo, GLuint& ibo, int& count) {
        count = mesh.indexCount();
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex),
                     mesh.vertices.data(), GL_STATIC_DRAW);
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned short),
                     mesh.indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void drawMesh(GLuint vbo, GLuint ibo, int count, const Mat4& mvp, const Mat4& model) {
        glUniformMatrix4fv(uMVP, 1, GL_FALSE, mvp.data());
        glUniformMatrix4fv(uModel, 1, GL_FALSE, model.data());

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        glEnableVertexAttribArray(aPosition);
        glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(aNormal);
        glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)12);

        glEnableVertexAttribArray(aColor);
        glVertexAttribPointer(aColor, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)24);

        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);
    }

    void initScenery() {
        treePositions.clear();
        buildingPositions.clear();
        for (int i = 0; i < 20; i++) {
            buildingHeights[i] = 5.0f + (float)(rand() % 15);
        }
        // Place initial trees and buildings
        for (float z = -100; z < 0; z += 15) {
            treePositions.push_back(Vec3(-9.0f + (float)(rand()%3), 0, z));
            treePositions.push_back(Vec3(9.0f + (float)(rand()%3), 0, z));
        }
        for (float z = -120; z < 0; z += 25) {
            buildingPositions.push_back(Vec3(-16.0f, 0, z));
            buildingPositions.push_back(Vec3(16.0f, 0, z));
        }
        lastTreeSpawnZ = 0;
        lastBuildingSpawnZ = 0;
    }

public:
    Renderer3D() : engine(nullptr), screenWidth(0), screenHeight(0), aspect(1.0f) {}

    void init() {
        // Create shader programs
        mainProgram = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
        skyProgram = createProgram(SKY_VERTEX_SHADER, SKY_FRAGMENT_SHADER);

        // Get attribute locations
        aPosition = glGetAttribLocation(mainProgram, "aPosition");
        aNormal = glGetAttribLocation(mainProgram, "aNormal");
        aColor = glGetAttribLocation(mainProgram, "aColor");

        skyAPosition = glGetAttribLocation(skyProgram, "aPosition");

        // Get uniform locations
        uMVP = glGetUniformLocation(mainProgram, "uMVP");
        uModel = glGetUniformLocation(mainProgram, "uModel");
        uLightDir = glGetUniformLocation(mainProgram, "uLightDir");
        uAmbientColor = glGetUniformLocation(mainProgram, "uAmbientColor");
        uFogColor = glGetUniformLocation(mainProgram, "uFogColor");
        uFogNear = glGetUniformLocation(mainProgram, "uFogNear");
        uFogFar = glGetUniformLocation(mainProgram, "uFogFar");

        skyUTopColor = glGetUniformLocation(skyProgram, "uTopColor");
        skyUBottomColor = glGetUniformLocation(skyProgram, "uBottomColor");

        // Upload meshes to GPU
        Mesh car = createPlayerCarMesh();
        uploadMesh(car, playerCarVBO, playerCarIBO, playerCarIndexCount);

        Mesh obsCar = createObstacleCarMesh(0.85f, 0.15f, 0.15f);
        uploadMesh(obsCar, obstacleCarVBO, obstacleCarIBO, obstacleCarIndexCount);

        Mesh truck = createTruckMesh();
        uploadMesh(truck, truckVBO, truckIBO, truckIndexCount);

        Mesh coin = createCoinMesh();
        uploadMesh(coin, coinVBO, coinIBO, coinIndexCount);

        Mesh road = createRoadSegment();
        uploadMesh(road, roadVBO, roadIBO, roadIndexCount);

        Mesh tree = createTreeMesh();
        uploadMesh(tree, treeVBO, treeIBO, treeIndexCount);

        Mesh building = createBuildingMesh(10.0f);
        uploadMesh(building, buildingVBO, buildingIBO, buildingIndexCount);

        Mesh ground = createGroundPlane();
        uploadMesh(ground, groundVBO, groundIBO, groundIndexCount);

        // Sky quad
        float skyVerts[] = {
            -1,-1,0,  1,-1,0,  1,1,0,  -1,1,0
        };
        glGenBuffers(1, &skyVBO);
        glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyVerts), skyVerts, GL_STATIC_DRAW);

        initScenery();

        // GL state
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    }

    void setEngine(Game3DEngine* e) { engine = e; }

    void resize(int w, int h) {
        screenWidth = w;
        screenHeight = h;
        aspect = (float)w / (float)h;
        glViewport(0, 0, w, h);
    }

    void render() {
        if (!engine) return;

        const Game3DState& st = engine->getState();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // === Camera ===
        Vec3 playerPos = st.playerPos;
        cameraPos = Vec3(playerPos.x * 0.7f, 6.0f, playerPos.z + 14.0f);
        cameraTarget = Vec3(playerPos.x * 0.9f, 1.5f, playerPos.z - 10.0f);

        Mat4 proj = Mat4::perspective(60.0f, aspect, 0.5f, 300.0f);
        Mat4 view = Mat4::lookAt(cameraPos, cameraTarget, Vec3(0,1,0));
        Mat4 vp = proj * view;

        // === Draw Sky ===
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(skyProgram);
        glUniform3f(skyUTopColor, 0.3f, 0.5f, 0.95f);
        glUniform3f(skyUBottomColor, 0.7f, 0.85f, 1.0f);
        glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
        glEnableVertexAttribArray(skyAPosition);
        glVertexAttribPointer(skyAPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glDisableVertexAttribArray(skyAPosition);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // === Main rendering ===
        glUseProgram(mainProgram);
        glUniform3f(uLightDir, 0.4f, 0.8f, 0.3f);
        glUniform3f(uAmbientColor, 0.35f, 0.35f, 0.35f);
        glUniform3f(uFogColor, 0.7f, 0.85f, 1.0f);
        glUniform1f(uFogNear, 80.0f);
        glUniform1f(uFogFar, 200.0f);

        // === Ground ===
        {
            Mat4 model = Mat4::translate(playerPos.x, -0.05f, playerPos.z - 200.0f);
            Mat4 mvp = vp * model;
            drawMesh(groundVBO, groundIBO, groundIndexCount, mvp, model);
        }

        // === Road segments ===
        float roadBaseZ = fmodf(playerPos.z, 20.0f);
        for (int i = -3; i < 30; i++) {
            float z = playerPos.z - (float)i * 20.0f + roadBaseZ;
            Mat4 model = Mat4::translate(0, 0, z);
            Mat4 mvp = vp * model;
            drawMesh(roadVBO, roadIBO, roadIndexCount, mvp, model);
        }

        // === Player Car ===
        {
            Mat4 model = Mat4::translate(playerPos.x, 0, playerPos.z)
                       * Mat4::rotateY(st.playerAngle);
            Mat4 mvp = vp * model;
            // Shield glow color change
            if (st.shield) {
                // We'll just render the same car - color change would require re-uploading
            }
            drawMesh(playerCarVBO, playerCarIBO, playerCarIndexCount, mvp, model);
        }

        // === Obstacles ===
        for (auto& obs : st.obstacles) {
            if (!obs.active) continue;

            Mat4 model;
            Mat4 mvp;

            switch (obs.type) {
                case OBJ_CAR_SLOW:
                    model = Mat4::translate(obs.pos.x, 0, obs.pos.z);
                    mvp = vp * model;
                    drawMesh(obstacleCarVBO, obstacleCarIBO, obstacleCarIndexCount, mvp, model);
                    break;
                case OBJ_CAR_FAST:
                    model = Mat4::translate(obs.pos.x, 0, obs.pos.z);
                    mvp = vp * model;
                    // Reuse same mesh but we'll change approach - render purple car differently
                    // For simplicity, render same red car for now
                    drawMesh(obstacleCarVBO, obstacleCarIBO, obstacleCarIndexCount, mvp, model);
                    break;
                case OBJ_TRUCK:
                    model = Mat4::translate(obs.pos.x, 0, obs.pos.z);
                    mvp = vp * model;
                    drawMesh(truckVBO, truckIBO, truckIndexCount, mvp, model);
                    break;
                case OBJ_COIN:
                    model = Mat4::translate(obs.pos.x, 0.5f, obs.pos.z)
                          * Mat4::rotateY(st.distance * 90.0f)  // spinning
                          * Mat4::rotateX(90.0f);  // face player
                    mvp = vp * model;
                    drawMesh(coinVBO, coinIBO, coinIndexCount, mvp, model);
                    break;
            }
        }

        // === Trees ===
        updateScenery();
        for (auto& tp : treePositions) {
            float dist = fabsf(tp.z - playerPos.z);
            if (dist > 120.0f) continue;
            Mat4 model = Mat4::translate(tp.x, 0, tp.z);
            Mat4 mvp = vp * model;
            drawMesh(treeVBO, treeIBO, treeIndexCount, mvp, model);
        }

        // === Buildings ===
        for (int i = 0; i < (int)buildingPositions.size(); i++) {
            auto& bp = buildingPositions[i];
            float dist = fabsf(bp.z - playerPos.z);
            if (dist > 150.0f) continue;
            float h = buildingHeights[i % 20];
            Mat4 model = Mat4::translate(bp.x, 0, bp.z)
                       * Mat4::scale(1.0f, h/10.0f, 1.0f);
            Mat4 mvp = vp * model;
            drawMesh(buildingVBO, buildingIBO, buildingIndexCount, mvp, model);
        }

        // === Exhaust particles (nitro effect) ===
        if (st.nitroTimer > 0) {
            // Add small boxes behind car as exhaust
            for (int i = 0; i < 3; i++) {
                float offZ = 2.5f + i * 0.8f + (float)(rand()%10)*0.1f;
                float offX = -0.5f + (float)(rand()%10)*0.1f;
                Mat4 model = Mat4::translate(playerPos.x + offX, 0.3f, playerPos.z + offZ)
                           * Mat4::scale(0.3f, 0.3f, 0.3f);
                Mat4 mvp = vp * model;
                drawMesh(coinVBO, coinIBO, coinIndexCount, mvp, model); // reuse coin mesh
            }
        }
    }

    void updateScenery() {
        if (!engine) return;
        float pz = engine->getState().playerPos.z;

        // Add new trees ahead
        while (lastTreeSpawnZ > pz - 200) {
            lastTreeSpawnZ -= 15.0f;
            treePositions.push_back(Vec3(-9.0f + (float)(rand()%4), 0, lastTreeSpawnZ));
            treePositions.push_back(Vec3(9.0f + (float)(rand()%4), 0, lastTreeSpawnZ));
        }

        // Add new buildings ahead
        while (lastBuildingSpawnZ > pz - 250) {
            lastBuildingSpawnZ -= 25.0f;
            buildingPositions.push_back(Vec3(-16.0f - (float)(rand()%5), 0, lastBuildingSpawnZ));
            buildingPositions.push_back(Vec3(16.0f + (float)(rand()%5), 0, lastBuildingSpawnZ));
        }

        // Remove old trees behind player
        for (auto it = treePositions.begin(); it != treePositions.end(); ) {
            if (it->z > pz + 50) it = treePositions.erase(it);
            else it++;
        }

        // Remove old buildings
        for (auto it = buildingPositions.begin(); it != buildingPositions.end(); ) {
            if (it->z > pz + 50) it = buildingPositions.erase(it);
            else it++;
        }
    }

    void cleanup() {
        glDeleteBuffers(1, &playerCarVBO); glDeleteBuffers(1, &playerCarIBO);
        glDeleteBuffers(1, &obstacleCarVBO); glDeleteBuffers(1, &obstacleCarIBO);
        glDeleteBuffers(1, &truckVBO); glDeleteBuffers(1, &truckIBO);
        glDeleteBuffers(1, &coinVBO); glDeleteBuffers(1, &coinIBO);
        glDeleteBuffers(1, &roadVBO); glDeleteBuffers(1, &roadIBO);
        glDeleteBuffers(1, &treeVBO); glDeleteBuffers(1, &treeIBO);
        glDeleteBuffers(1, &buildingVBO); glDeleteBuffers(1, &buildingIBO);
        glDeleteBuffers(1, &groundVBO); glDeleteBuffers(1, &groundIBO);
        glDeleteBuffers(1, &skyVBO);
        glDeleteProgram(mainProgram);
        glDeleteProgram(skyProgram);
    }
};

#endif // RENDERER3D_H
