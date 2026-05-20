/*
 * ============================================
 *   PROCEDURAL 3D MESH GENERATION
 *   Cars, Road, Trees, Buildings - Enhanced
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
#define LIME    0.3f,0.9f,0.3f,1.0f
#define STEEL   0.6f,0.6f,0.65f,1.0f
#define GLASS   0.5f,0.7f,0.9f,0.6f

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

    // Add a sloped box (like a car hood) - trapezoid shape
    void addSlopedBox(float cx, float cy, float cz, float sx, float sy, float sz,
                      float slopeFront, float slopeBack,
                      float r, float g, float b, float a) {
        float hx=sx/2, hy=sy/2, hz=sz/2;
        float fOff = slopeFront;
        float bOff = slopeBack;
        // Front face (sloped)
        addQuad(Vec3(cx-hx,cy-hy,cz+hz), Vec3(cx+hx,cy-hy,cz+hz),
                Vec3(cx+hx,cy+hy+fOff,cz+hz), Vec3(cx-hx,cy+hy+fOff,cz+hz),
                Vec3(0,fOff*0.3f,1), r,g,b,a);
        // Back face (sloped)
        addQuad(Vec3(cx+hx,cy-hy,cz-hz), Vec3(cx-hx,cy-hy,cz-hz),
                Vec3(cx-hx,cy+hy+bOff,cz-hz), Vec3(cx+hx,cy+hy+bOff,cz-hz),
                Vec3(0,bOff*0.3f,-1), r,g,b,a);
        // Top (sloped)
        addQuad(Vec3(cx-hx,cy+hy+fOff,cz+hz), Vec3(cx+hx,cy+hy+fOff,cz+hz),
                Vec3(cx+hx,cy+hy+bOff,cz-hz), Vec3(cx-hx,cy+hy+bOff,cz-hz),
                Vec3(0,1,0), r*1.05f,g*1.05f,b*1.05f,a);
        // Bottom
        addQuad(Vec3(cx-hx,cy-hy,cz-hz), Vec3(cx+hx,cy-hy,cz-hz),
                Vec3(cx+hx,cy-hy,cz+hz), Vec3(cx-hx,cy-hy,cz+hz),
                Vec3(0,-1,0), r*0.7f,g*0.7f,b*0.7f,a);
        // Right side
        addQuad(Vec3(cx+hx,cy-hy,cz+hz), Vec3(cx+hx,cy-hy,cz-hz),
                Vec3(cx+hx,cy+hy+bOff,cz-hz), Vec3(cx+hx,cy+hy+fOff,cz+hz),
                Vec3(1,0,0), r*0.85f,g*0.85f,b*0.85f,a);
        // Left side
        addQuad(Vec3(cx-hx,cy-hy,cz-hz), Vec3(cx-hx,cy-hy,cz+hz),
                Vec3(cx-hx,cy+hy+fOff,cz+hz), Vec3(cx-hx,cy+hy+bOff,cz-hz),
                Vec3(-1,0,0), r*0.85f,g*0.85f,b*0.85f,a);
    }

    void clear() { vertices.clear(); indices.clear(); }
    int indexCount() const { return (int)indices.size(); }
};

// ====== Enhanced Player Car Mesh (Sports Car) ======
inline Mesh createPlayerCarMesh() {
    Mesh m;

    // Lower body / chassis - sleek sports car shape
    m.addSlopedBox(0, 0.25f, 0, 1.9f, 0.35f, 4.2f, 0.15f, -0.05f, GREEN);
    // Undercarriage
    m.addBox(0, 0.1f, 0, 1.6f, 0.08f, 3.8f, DGRAY);

    // Cabin / cockpit
    m.addBox(0, 0.7f, -0.15f, 1.4f, 0.4f, 1.8f, 0.18f,0.5f,0.18f,1.0f);

    // Windshield (front glass) - angled
    m.addSlopedBox(0, 0.6f, 0.75f, 1.3f, 0.3f, 0.3f, 0.2f, -0.15f, GLASS);

    // Rear window
    m.addSlopedBox(0, 0.6f, -1.1f, 1.3f, 0.25f, 0.2f, -0.1f, 0.15f, GLASS);

    // Front bumper
    m.addBox(0, 0.2f, 2.1f, 1.85f, 0.15f, 0.3f, STEEL);

    // Rear bumper
    m.addBox(0, 0.2f, -2.1f, 1.85f, 0.15f, 0.3f, STEEL);

    // Hood scoop
    m.addBox(0, 0.55f, 1.2f, 0.4f, 0.1f, 0.6f, DGRAY);

    // Headlights (LED style)
    m.addBox(-0.7f, 0.3f, 2.15f, 0.35f, 0.12f, 0.08f, 1.0f,1.0f,0.9f,1.0f);
    m.addBox(0.7f, 0.3f, 2.15f, 0.35f, 0.12f, 0.08f, 1.0f,1.0f,0.9f,1.0f);

    // Taillights (wide LED bar)
    m.addBox(-0.65f, 0.3f, -2.15f, 0.5f, 0.08f, 0.05f, 1.0f,0.05f,0.05f,1.0f);
    m.addBox(0.65f, 0.3f, -2.15f, 0.5f, 0.08f, 0.05f, 1.0f,0.05f,0.05f,1.0f);
    // Tail light bar
    m.addBox(0, 0.3f, -2.15f, 0.4f, 0.05f, 0.05f, 1.0f,0.05f,0.05f,1.0f);

    // Side mirrors
    m.addBox(-1.05f, 0.65f, 0.5f, 0.15f, 0.1f, 0.15f, DGRAY);
    m.addBox(1.05f, 0.65f, 0.5f, 0.15f, 0.1f, 0.15f, DGRAY);

    // Wheels (4 wheels with detail)
    // Front wheels
    m.addBox(-1.0f, 0.15f, 1.2f, 0.3f, 0.3f, 0.5f, BLACK);
    m.addBox(1.0f, 0.15f, 1.2f, 0.3f, 0.3f, 0.5f, BLACK);
    // Rear wheels
    m.addBox(-1.0f, 0.15f, -1.2f, 0.3f, 0.3f, 0.5f, BLACK);
    m.addBox(1.0f, 0.15f, -1.2f, 0.3f, 0.3f, 0.5f, BLACK);
    // Wheel rims
    m.addBox(-1.0f, 0.16f, 1.2f, 0.2f, 0.2f, 0.52f, STEEL);
    m.addBox(1.0f, 0.16f, 1.2f, 0.2f, 0.2f, 0.52f, STEEL);
    m.addBox(-1.0f, 0.16f, -1.2f, 0.2f, 0.2f, 0.52f, STEEL);
    m.addBox(1.0f, 0.16f, -1.2f, 0.2f, 0.2f, 0.52f, STEEL);

    // Side skirts
    m.addBox(-0.95f, 0.12f, 0, 0.08f, 0.1f, 3.5f, DGRAY);
    m.addBox(0.95f, 0.12f, 0, 0.08f, 0.1f, 3.5f, DGRAY);

    // Rear spoiler
    m.addBox(0, 0.85f, -1.9f, 1.5f, 0.05f, 0.3f, DGRAY);
    m.addBox(-0.7f, 0.75f, -1.85f, 0.08f, 0.15f, 0.1f, DGRAY);
    m.addBox(0.7f, 0.75f, -1.85f, 0.08f, 0.15f, 0.1f, DGRAY);

    // Exhaust pipes
    m.addBox(-0.5f, 0.1f, -2.2f, 0.12f, 0.12f, 0.15f, STEEL);
    m.addBox(0.5f, 0.1f, -2.2f, 0.12f, 0.12f, 0.15f, STEEL);

    return m;
}

// ====== Enhanced Obstacle Car Mesh ======
inline Mesh createObstacleCarMesh(float r, float g, float b) {
    Mesh m;
    // Sedan body
    m.addSlopedBox(0, 0.3f, 0, 1.7f, 0.4f, 3.7f, 0.1f, -0.05f, r,g,b,1.0f);
    // Cabin
    m.addBox(0, 0.75f, -0.15f, 1.3f, 0.35f, 1.7f, r*0.6f,g*0.6f,b*0.6f,1.0f);
    // Windshield
    m.addSlopedBox(0, 0.65f, 0.6f, 1.2f, 0.25f, 0.2f, 0.15f, -0.1f, GLASS);
    // Rear window
    m.addSlopedBox(0, 0.65f, -1.0f, 1.2f, 0.2f, 0.15f, -0.08f, 0.1f, GLASS);
    // Headlights
    m.addBox(-0.6f, 0.3f, 1.85f, 0.3f, 0.15f, 0.08f, 1.0f,1.0f,0.85f,1.0f);
    m.addBox(0.6f, 0.3f, 1.85f, 0.3f, 0.15f, 0.08f, 1.0f,1.0f,0.85f,1.0f);
    // Taillights
    m.addBox(-0.6f, 0.3f, -1.85f, 0.3f, 0.12f, 0.05f, 1.0f,0.08f,0.08f,1.0f);
    m.addBox(0.6f, 0.3f, -1.85f, 0.3f, 0.12f, 0.05f, 1.0f,0.08f,0.08f,1.0f);
    // Wheels
    m.addBox(-0.9f, 0.15f, 1.0f, 0.28f, 0.28f, 0.45f, BLACK);
    m.addBox(0.9f, 0.15f, 1.0f, 0.28f, 0.28f, 0.45f, BLACK);
    m.addBox(-0.9f, 0.15f, -1.0f, 0.28f, 0.28f, 0.45f, BLACK);
    m.addBox(0.9f, 0.15f, -1.0f, 0.28f, 0.28f, 0.45f, BLACK);
    // Rims
    m.addBox(-0.9f, 0.16f, 1.0f, 0.18f, 0.18f, 0.47f, STEEL);
    m.addBox(0.9f, 0.16f, 1.0f, 0.18f, 0.18f, 0.47f, STEEL);
    m.addBox(-0.9f, 0.16f, -1.0f, 0.18f, 0.18f, 0.47f, STEEL);
    m.addBox(0.9f, 0.16f, -1.0f, 0.18f, 0.18f, 0.47f, STEEL);
    return m;
}

// ====== Enhanced Truck Mesh ======
inline Mesh createTruckMesh() {
    Mesh m;
    // Cargo container
    m.addBox(0, 1.0f, -0.5f, 2.3f, 1.6f, 4.8f, ORANGE);
    // Container ridges (detail lines)
    for (float z = -2.0f; z <= 1.0f; z += 1.5f) {
        m.addBox(-1.16f, 1.0f, z, 0.02f, 1.5f, 0.1f, 0.7f,0.35f,0.05f,1.0f);
        m.addBox(1.16f, 1.0f, z, 0.02f, 1.5f, 0.1f, 0.7f,0.35f,0.05f,1.0f);
    }
    // Cab
    m.addSlopedBox(0, 0.7f, 2.2f, 2.1f, 1.0f, 1.8f, 0.15f, 0.0f, 0.7f,0.4f,0.1f,1.0f);
    // Windshield
    m.addSlopedBox(0, 0.8f, 3.0f, 1.7f, 0.5f, 0.15f, 0.2f, -0.1f, GLASS);
    // Bumper
    m.addBox(0, 0.25f, 3.05f, 2.2f, 0.2f, 0.2f, STEEL);
    // Headlights
    m.addBox(-0.85f, 0.35f, 3.1f, 0.3f, 0.2f, 0.08f, 1.0f,1.0f,0.85f,1.0f);
    m.addBox(0.85f, 0.35f, 3.1f, 0.3f, 0.2f, 0.08f, 1.0f,1.0f,0.85f,1.0f);
    // Wheels (6 wheels - dual rear axle)
    m.addBox(-1.15f, 0.2f, 2.0f, 0.35f, 0.4f, 0.6f, BLACK);
    m.addBox(1.15f, 0.2f, 2.0f, 0.35f, 0.4f, 0.6f, BLACK);
    m.addBox(-1.15f, 0.2f, -1.2f, 0.35f, 0.4f, 0.6f, BLACK);
    m.addBox(1.15f, 0.2f, -1.2f, 0.35f, 0.4f, 0.6f, BLACK);
    m.addBox(-1.15f, 0.2f, -2.3f, 0.35f, 0.4f, 0.6f, BLACK);
    m.addBox(1.15f, 0.2f, -2.3f, 0.35f, 0.4f, 0.6f, BLACK);
    // Rims
    m.addBox(-1.15f, 0.22f, 2.0f, 0.22f, 0.28f, 0.62f, STEEL);
    m.addBox(1.15f, 0.22f, 2.0f, 0.22f, 0.28f, 0.62f, STEEL);
    m.addBox(-1.15f, 0.22f, -1.2f, 0.22f, 0.28f, 0.62f, STEEL);
    m.addBox(1.15f, 0.22f, -1.2f, 0.22f, 0.28f, 0.62f, STEEL);
    m.addBox(-1.15f, 0.22f, -2.3f, 0.22f, 0.28f, 0.62f, STEEL);
    m.addBox(1.15f, 0.22f, -2.3f, 0.22f, 0.28f, 0.62f, STEEL);
    return m;
}

// ====== Enhanced Coin Mesh (Rotating Octagon) ======
inline Mesh createCoinMesh() {
    Mesh m;
    // Coin disc - use two flat boxes cross-shaped for rotating effect
    m.addBox(0, 0, 0, 1.0f, 1.0f, 0.12f, GOLD);
    m.addBox(0, 0, 0, 0.12f, 1.0f, 1.0f, GOLD);
    // Inner circle detail
    m.addBox(0, 0, 0.06f, 0.6f, 0.6f, 0.02f, 1.0f,0.95f,0.3f,1.0f);
    m.addBox(0, 0, -0.06f, 0.6f, 0.6f, 0.02f, 1.0f,0.95f,0.3f,1.0f);
    return m;
}

// ====== Enhanced Road Segment ======
inline Mesh createRoadSegment() {
    Mesh m;
    // Main road surface
    m.addBox(0, 0.01f, 0, 12.0f, 0.02f, 20.0f, ROAD_C);

    // Center line dashes (yellow)
    for (float z = -8.0f; z <= 8.0f; z += 5.0f) {
        m.addBox(0, 0.025f, z, 0.2f, 0.01f, 2.0f, YELLOW);
    }

    // Lane divider lines (white dashes)
    for (float z = -8.0f; z <= 8.0f; z += 5.0f) {
        m.addBox(-3.0f, 0.025f, z, 0.15f, 0.01f, 2.0f, WHITE);
        m.addBox(3.0f, 0.025f, z, 0.15f, 0.01f, 2.0f, WHITE);
    }

    // Road edge lines (solid white)
    m.addBox(-5.8f, 0.025f, 0, 0.15f, 0.01f, 20.0f, WHITE);
    m.addBox(5.8f, 0.025f, 0, 0.15f, 0.01f, 20.0f, WHITE);

    // Sidewalks / curbs
    m.addBox(-6.5f, 0.12f, 0, 1.0f, 0.25f, 20.0f, CONCRETE);
    m.addBox(6.5f, 0.12f, 0, 1.0f, 0.25f, 20.0f, CONCRETE);

    // Guard rails (metal barriers on road edges)
    // Left guardrail
    m.addBox(-6.2f, 0.45f, 0, 0.08f, 0.5f, 20.0f, STEEL);
    m.addBox(-6.2f, 0.25f, 0, 0.15f, 0.08f, 20.0f, STEEL);
    // Guardrail posts
    for (float z = -9.0f; z <= 9.0f; z += 3.0f) {
        m.addBox(-6.2f, 0.25f, z, 0.1f, 0.5f, 0.1f, GRAY);
    }

    // Right guardrail
    m.addBox(6.2f, 0.45f, 0, 0.08f, 0.5f, 20.0f, STEEL);
    m.addBox(6.2f, 0.25f, 0, 0.15f, 0.08f, 20.0f, STEEL);
    for (float z = -9.0f; z <= 9.0f; z += 3.0f) {
        m.addBox(6.2f, 0.25f, z, 0.1f, 0.5f, 0.1f, GRAY);
    }

    return m;
}

// ====== Enhanced Tree Mesh ======
inline Mesh createTreeMesh() {
    Mesh m;
    // Trunk (tapered)
    m.addBox(0, 1.2f, 0, 0.35f, 2.4f, 0.35f, SKIN);
    m.addBox(0, 2.6f, 0, 0.2f, 0.5f, 0.2f, SKIN);
    // Foliage (layered cones)
    m.addBox(0, 3.5f, 0, 2.8f, 1.8f, 2.8f, DGREEN);
    m.addBox(0, 4.6f, 0, 2.2f, 1.5f, 2.2f, GREEN);
    m.addBox(0, 5.5f, 0, 1.4f, 1.2f, 1.4f, 0.2f,0.65f,0.2f,1.0f);
    m.addBox(0, 6.2f, 0, 0.7f, 0.8f, 0.7f, LIME);
    return m;
}

// ====== Enhanced Building Mesh ======
inline Mesh createBuildingMesh(float height) {
    Mesh m;
    float h = height;
    float gray = 0.4f + (float)(rand()%30)/100.0f;

    // Main structure
    m.addBox(0, h/2, 0, 4.5f, h, 4.5f, gray, gray*0.95f, gray*0.9f, 1.0f);

    // Roof detail
    m.addBox(0, h+0.1f, 0, 4.8f, 0.15f, 4.8f, DGRAY);

    // Windows on each side (lit/unlit variation)
    for (int wy = 1; wy < (int)h-1; wy += 2) {
        for (int wz = -1; wz <= 1; wz += 2) {
            float windowAlpha = (rand() % 3 == 0) ? 0.5f : 0.9f;  // some windows dim
            float windowR = (rand() % 2 == 0) ? 0.7f : 0.9f;     // warm/cool light
            // Left side windows
            m.addBox(-2.26f, (float)wy, (float)wz, 0.05f, 1.0f, 0.9f,
                     windowR,0.85f,1.0f,windowAlpha);
            // Right side windows
            m.addBox(2.26f, (float)wy, (float)wz, 0.05f, 1.0f, 0.9f,
                     windowR,0.85f,1.0f,windowAlpha);
            // Front windows
            m.addBox((float)wz, (float)wy, 2.26f, 0.9f, 1.0f, 0.05f,
                     windowR,0.85f,1.0f,windowAlpha);
            // Back windows
            m.addBox((float)wz, (float)wy, -2.26f, 0.9f, 1.0f, 0.05f,
                     windowR,0.85f,1.0f,windowAlpha);
        }
    }

    // Entrance (ground floor, front)
    m.addBox(0, 0.8f, 2.26f, 1.0f, 1.5f, 0.05f, 0.2f,0.15f,0.1f,1.0f);

    return m;
}

// ====== Enhanced Ground Plane ======
inline Mesh createGroundPlane() {
    Mesh m;
    // Main grass
    m.addBox(0, -0.05f, 0, 200.0f, 0.1f, 600.0f, 0.12f,0.35f,0.1f,1.0f);
    // Dirt edge on left
    m.addBox(-14.0f, -0.03f, 0, 5.0f, 0.06f, 600.0f, 0.35f,0.25f,0.15f,1.0f);
    // Dirt edge on right
    m.addBox(14.0f, -0.03f, 0, 5.0f, 0.06f, 600.0f, 0.35f,0.25f,0.15f,1.0f);
    return m;
}

#endif // MESH_H
