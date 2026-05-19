/*
 * ============================================
 *   PROCEDURAL 3D MESH GENERATION
 *   Cars, Road, Trees, Buildings
 * ============================================
 */
#ifndef MESH_H
#define MESH_H

#include "math3d.h"
#include <vector>

// Color helpers
#define RED     0.85f,0.15f,0.15f,1.0f
#define GREEN   0.15f,0.65f,0.15f,1.0f
#define BLUE    0.15f,0.15f,0.85f,1.0f
#define YELLOW  0.9f,0.8f,0.1f,1.0f
#define WHITE   0.95f,0.95f,0.95f,1.0f
#define GRAY    0.4f,0.4f,0.4f,1.0f
#define DGRAY   0.25f,0.25f,0.25f,1.0f
#define BLACK   0.05f,0.05f,0.05f,1.0f
#define ORANGE  0.9f,0.5f,0.1f,1.0f
#define PURPLE  0.6f,0.15f,0.8f,1.0f
#define CYAN    0.1f,0.75f,0.8f,1.0f
#define DGREEN  0.1f,0.35f,0.1f,1.0f
#define SKIN    0.35f,0.3f,0.25f,1.0f
#define ROAD_C  0.3f,0.3f,0.32f,1.0f
#define CONCRETE 0.55f,0.52f,0.5f,1.0f
#define GOLD    1.0f,0.84f,0.0f,1.0f

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;

    void addQuad(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3,
                 const Vec3& normal, float r, float g, float b, float a) {
        unsigned short base = (unsigned short)vertices.size();
        Vec3 n = normal.normalized();
        vertices.push_back({{p0.x,p0.y,p0.z},{n.x,n.y,n.z},{r,g,b,a}});
        vertices.push_back({{p1.x,p1.y,p1.z},{n.x,n.y,n.z},{r,g,b,a}});
        vertices.push_back({{p2.x,p2.y,p2.z},{n.x,n.y,n.z},{r,g,b,a}});
        vertices.push_back({{p3.x,p3.y,p3.z},{n.x,n.y,n.z},{r,g,b,a}});
        indices.push_back(base); indices.push_back(base+1); indices.push_back(base+2);
        indices.push_back(base); indices.push_back(base+2); indices.push_back(base+3);
    }

    void addBox(float cx, float cy, float cz, float sx, float sy, float sz,
                float r, float g, float b, float a) {
        float hx=sx/2, hy=sy/2, hz=sz/2;
        // Front
        addQuad(Vec3(cx-hx,cy-hy,cz+hz), Vec3(cx+hx,cy-hy,cz+hz),
                Vec3(cx+hx,cy+hy,cz+hz), Vec3(cx-hx,cy+hy,cz+hz),
                Vec3(0,0,1), r,g,b,a);
        // Back
        addQuad(Vec3(cx+hx,cy-hy,cz-hz), Vec3(cx-hx,cy-hy,cz-hz),
                Vec3(cx-hx,cy+hy,cz-hz), Vec3(cx+hx,cy+hy,cz-hz),
                Vec3(0,0,-1), r,g,b,a);
        // Top
        addQuad(Vec3(cx-hx,cy+hy,cz+hz), Vec3(cx+hx,cy+hy,cz+hz),
                Vec3(cx+hx,cy+hy,cz-hz), Vec3(cx-hx,cy+hy,cz-hz),
                Vec3(0,1,0), r,g,b,a);
        // Bottom
        addQuad(Vec3(cx-hx,cy-hy,cz-hz), Vec3(cx+hx,cy-hy,cz-hz),
                Vec3(cx+hx,cy-hy,cz+hz), Vec3(cx-hx,cy-hy,cz+hz),
                Vec3(0,-1,0), r*0.7f,g*0.7f,b*0.7f,a);
        // Right
        addQuad(Vec3(cx+hx,cy-hy,cz+hz), Vec3(cx+hx,cy-hy,cz-hz),
                Vec3(cx+hx,cy+hy,cz-hz), Vec3(cx+hx,cy+hy,cz+hz),
                Vec3(1,0,0), r*0.85f,g*0.85f,b*0.85f,a);
        // Left
        addQuad(Vec3(cx-hx,cy-hy,cz-hz), Vec3(cx-hx,cy-hy,cz+hz),
                Vec3(cx-hx,cy+hy,cz+hz), Vec3(cx-hx,cy+hy,cz-hz),
                Vec3(-1,0,0), r*0.85f,g*0.85f,b*0.85f,a);
    }

    void clear() { vertices.clear(); indices.clear(); }
    int indexCount() const { return (int)indices.size(); }
};

// ====== Car Mesh ======
inline Mesh createPlayerCarMesh() {
    Mesh m;
    // Body
    m.addBox(0, 0.4f, 0, 1.8f, 0.5f, 3.8f, GREEN);
    // Roof / cabin
    m.addBox(0, 0.85f, -0.2f, 1.4f, 0.4f, 2.0f, 0.2f,0.55f,0.2f,1.0f);
    // Windshield
    m.addBox(0, 0.75f, 0.7f, 1.3f, 0.35f, 0.1f, 0.5f,0.7f,0.9f,0.7f);
    // Rear window
    m.addBox(0, 0.75f, -1.1f, 1.3f, 0.3f, 0.1f, 0.4f,0.6f,0.8f,0.7f);
    // Headlights
    m.addBox(-0.65f, 0.35f, 1.9f, 0.3f, 0.2f, 0.1f, 1.0f,1.0f,0.8f,1.0f);
    m.addBox(0.65f, 0.35f, 1.9f, 0.3f, 0.2f, 0.1f, 1.0f,1.0f,0.8f,1.0f);
    // Taillights
    m.addBox(-0.65f, 0.35f, -1.9f, 0.3f, 0.15f, 0.05f, 1.0f,0.1f,0.1f,1.0f);
    m.addBox(0.65f, 0.35f, -1.9f, 0.3f, 0.15f, 0.05f, 1.0f,0.1f,0.1f,1.0f);
    // Wheels
    m.addBox(-0.95f, 0.15f, 1.1f, 0.3f, 0.3f, 0.5f, BLACK);
    m.addBox(0.95f, 0.15f, 1.1f, 0.3f, 0.3f, 0.5f, BLACK);
    m.addBox(-0.95f, 0.15f, -1.1f, 0.3f, 0.3f, 0.5f, BLACK);
    m.addBox(0.95f, 0.15f, -1.1f, 0.3f, 0.3f, 0.5f, BLACK);
    return m;
}

inline Mesh createObstacleCarMesh(float r, float g, float b) {
    Mesh m;
    m.addBox(0, 0.4f, 0, 1.7f, 0.5f, 3.6f, r,g,b,1.0f);
    m.addBox(0, 0.8f, -0.1f, 1.3f, 0.35f, 1.8f, r*0.7f,g*0.7f,b*0.7f,1.0f);
    m.addBox(0, 0.7f, 0.65f, 1.2f, 0.3f, 0.1f, 0.4f,0.6f,0.8f,0.7f);
    m.addBox(-0.6f, 0.35f, -1.8f, 0.25f, 0.15f, 0.05f, 1.0f,0.1f,0.1f,1.0f);
    m.addBox(0.6f, 0.35f, -1.8f, 0.25f, 0.15f, 0.05f, 1.0f,0.1f,0.1f,1.0f);
    m.addBox(-0.9f, 0.15f, 1.0f, 0.3f, 0.3f, 0.5f, BLACK);
    m.addBox(0.9f, 0.15f, 1.0f, 0.3f, 0.3f, 0.5f, BLACK);
    m.addBox(-0.9f, 0.15f, -1.0f, 0.3f, 0.3f, 0.5f, BLACK);
    m.addBox(0.9f, 0.15f, -1.0f, 0.3f, 0.3f, 0.5f, BLACK);
    return m;
}

inline Mesh createTruckMesh() {
    Mesh m;
    // Cargo
    m.addBox(0, 0.8f, -0.3f, 2.2f, 1.2f, 4.5f, ORANGE);
    // Cab
    m.addBox(0, 0.6f, 2.0f, 2.0f, 0.8f, 1.5f, 0.7f,0.4f,0.1f,1.0f);
    // Windshield
    m.addBox(0, 0.7f, 2.7f, 1.6f, 0.5f, 0.1f, 0.4f,0.6f,0.8f,0.7f);
    // Wheels
    m.addBox(-1.1f, 0.2f, 2.0f, 0.35f, 0.4f, 0.6f, BLACK);
    m.addBox(1.1f, 0.2f, 2.0f, 0.35f, 0.4f, 0.6f, BLACK);
    m.addBox(-1.1f, 0.2f, -1.0f, 0.35f, 0.4f, 0.6f, BLACK);
    m.addBox(1.1f, 0.2f, -1.0f, 0.35f, 0.4f, 0.6f, BLACK);
    m.addBox(-1.1f, 0.2f, -2.0f, 0.35f, 0.4f, 0.6f, BLACK);
    m.addBox(1.1f, 0.2f, -2.0f, 0.35f, 0.4f, 0.6f, BLACK);
    return m;
}

inline Mesh createCoinMesh() {
    Mesh m;
    // Flat disc (approximated as thin box)
    m.addBox(0, 0.8f, 0, 1.0f, 1.0f, 0.15f, GOLD);
    return m;
}

inline Mesh createRoadSegment() {
    Mesh m;
    // Road surface
    m.addBox(0, 0.01f, 0, 12.0f, 0.02f, 20.0f, ROAD_C);
    // Center line (yellow dashes)
    m.addBox(0, 0.02f, 0, 0.2f, 0.01f, 3.0f, YELLOW);
    m.addBox(0, 0.02f, 8.0f, 0.2f, 0.01f, 3.0f, YELLOW);
    m.addBox(0, 0.02f, -8.0f, 0.2f, 0.01f, 3.0f, YELLOW);
    // Lane lines
    m.addBox(-3.0f, 0.02f, 0, 0.15f, 0.01f, 3.0f, WHITE);
    m.addBox(-3.0f, 0.02f, 8.0f, 0.15f, 0.01f, 3.0f, WHITE);
    m.addBox(-3.0f, 0.02f, -8.0f, 0.15f, 0.01f, 3.0f, WHITE);
    m.addBox(3.0f, 0.02f, 0, 0.15f, 0.01f, 3.0f, WHITE);
    m.addBox(3.0f, 0.02f, 8.0f, 0.15f, 0.01f, 3.0f, WHITE);
    m.addBox(3.0f, 0.02f, -8.0f, 0.15f, 0.01f, 3.0f, WHITE);
    // Sidewalks
    m.addBox(-6.5f, 0.1f, 0, 1.0f, 0.2f, 20.0f, CONCRETE);
    m.addBox(6.5f, 0.1f, 0, 1.0f, 0.2f, 20.0f, CONCRETE);
    return m;
}

inline Mesh createTreeMesh() {
    Mesh m;
    // Trunk
    m.addBox(0, 1.5f, 0, 0.4f, 3.0f, 0.4f, SKIN);
    // Foliage layers
    m.addBox(0, 3.5f, 0, 2.5f, 2.0f, 2.5f, DGREEN);
    m.addBox(0, 4.8f, 0, 1.8f, 1.5f, 1.8f, GREEN);
    m.addBox(0, 5.8f, 0, 1.0f, 1.0f, 1.0f, 0.2f,0.7f,0.2f,1.0f);
    return m;
}

inline Mesh createBuildingMesh(float height) {
    Mesh m;
    float h = height;
    // Main building
    float gray = 0.4f + (float)(rand()%30)/100.0f;
    m.addBox(0, h/2, 0, 4.0f, h, 4.0f, gray, gray*0.95f, gray*0.9f, 1.0f);
    // Windows
    for(int wy = 1; wy < (int)h-1; wy += 2) {
        for(int wz = -1; wz <= 1; wz += 2) {
            m.addBox(-2.01f, (float)wy, (float)wz, 0.05f, 1.0f, 0.8f,
                     0.7f,0.85f,1.0f,0.8f);
            m.addBox(2.01f, (float)wy, (float)wz, 0.05f, 1.0f, 0.8f,
                     0.7f,0.85f,1.0f,0.8f);
        }
    }
    return m;
}

inline Mesh createGroundPlane() {
    Mesh m;
    m.addBox(0, -0.05f, 0, 200.0f, 0.1f, 600.0f, 0.15f,0.4f,0.12f,1.0f);
    return m;
}

#endif // MESH_H
