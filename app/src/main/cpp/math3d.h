/*
 * ============================================
 *   3D MATH LIBRARY - OpenGL ES 2.0
 *   vec3, vec4, mat4, utility functions
 * ============================================
 */
#ifndef MATH3D_H
#define MATH3D_H

#include <cmath>
#include <cstring>
#include <cstdio>

const float PI = 3.14159265358979323846f;
const float DEG2RAD = PI / 180.0f;

struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3 operator+(const Vec3& v) const { return Vec3(x+v.x, y+v.y, z+v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x-v.x, y-v.y, z-v.z); }
    Vec3 operator*(float s) const { return Vec3(x*s, y*s, z*s); }
    Vec3 operator/(float s) const { return Vec3(x/s, y/s, z/s); }
    Vec3 operator-() const { return Vec3(-x, -y, -z); }
    Vec3& operator+=(const Vec3& v) { x+=v.x; y+=v.y; z+=v.z; return *this; }
    float length() const { return sqrtf(x*x + y*y + z*z); }
    Vec3 normalized() const { float l=length(); return l>0 ? *this/l : Vec3(); }
    static float dot(const Vec3& a, const Vec3& b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
    static Vec3 cross(const Vec3& a, const Vec3& b) {
        return Vec3(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
    }
    static Vec3 lerp(const Vec3& a, const Vec3& b, float t) {
        return Vec3(a.x+(b.x-a.x)*t, a.y+(b.y-a.y)*t, a.z+(b.z-a.z)*t);
    }
};

// 4x4 Matrix - column major for OpenGL
struct Mat4 {
    float m[16]; // column-major

    Mat4() { identity(); }

    void identity() {
        memset(m, 0, sizeof(m));
        m[0]=m[5]=m[10]=m[15]=1.0f;
    }

    float& operator()(int row, int col) { return m[col*4+row]; }
    float operator()(int row, int col) const { return m[col*4+row]; }

    Mat4 operator*(const Mat4& b) const {
        Mat4 r;
        for(int col=0; col<4; col++)
            for(int row=0; row<4; row++) {
                r.m[col*4+row] = 0;
                for(int k=0; k<4; k++)
                    r.m[col*4+row] += m[k*4+row] * b.m[col*4+k];
            }
        return r;
    }

    static Mat4 perspective(float fovDeg, float aspect, float near, float far) {
        Mat4 r;
        memset(r.m, 0, sizeof(r.m));
        float f = 1.0f / tanf(fovDeg * DEG2RAD * 0.5f);
        r.m[0] = f / aspect;
        r.m[5] = f;
        r.m[10] = (far + near) / (near - far);
        r.m[11] = -1.0f;
        r.m[14] = (2.0f * far * near) / (near - far);
        return r;
    }

    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = (center - eye).normalized();
        Vec3 s = Vec3::cross(f, up).normalized();
        Vec3 u = Vec3::cross(s, f);
        Mat4 r;
        r.m[0]=s.x;  r.m[4]=s.y;  r.m[8]=s.z;   r.m[12]=-Vec3::dot(s,eye);
        r.m[1]=u.x;  r.m[5]=u.y;  r.m[9]=u.z;   r.m[13]=-Vec3::dot(u,eye);
        r.m[2]=-f.x; r.m[6]=-f.y; r.m[10]=-f.z;  r.m[14]=Vec3::dot(f,eye);
        r.m[3]=0;    r.m[7]=0;    r.m[11]=0;     r.m[15]=1;
        return r;
    }

    static Mat4 translate(float x, float y, float z) {
        Mat4 r;
        r.m[12]=x; r.m[13]=y; r.m[14]=z;
        return r;
    }

    static Mat4 scale(float x, float y, float z) {
        Mat4 r;
        r.m[0]=x; r.m[5]=y; r.m[10]=z;
        return r;
    }

    static Mat4 rotateY(float deg) {
        Mat4 r;
        float c = cosf(deg*DEG2RAD), s = sinf(deg*DEG2RAD);
        r.m[0]=c;  r.m[8]=s;
        r.m[2]=-s; r.m[10]=c;
        return r;
    }

    static Mat4 rotateX(float deg) {
        Mat4 r;
        float c = cosf(deg*DEG2RAD), s = sinf(deg*DEG2RAD);
        r.m[5]=c;  r.m[9]=-s;
        r.m[6]=s;  r.m[10]=c;
        return r;
    }

    static Mat4 rotateZ(float deg) {
        Mat4 r;
        float c = cosf(deg*DEG2RAD), s = sinf(deg*DEG2RAD);
        r.m[0]=c;  r.m[4]=-s;
        r.m[1]=s;  r.m[5]=c;
        return r;
    }

    const float* data() const { return m; }
};

// Vertex with position, normal, color
struct Vertex {
    float pos[3];
    float normal[3];
    float color[4];
};

#endif // MATH3D_H
