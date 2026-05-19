/*
 * ============================================
 *   GLSL SHADERS - OpenGL ES 2.0
 *   Phong Lighting + Color
 * ============================================
 */
#ifndef SHADERS_H
#define SHADERS_H

const char* VERTEX_SHADER = R"(
attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec4 aColor;

uniform mat4 uMVP;
uniform mat4 uModel;

varying vec3 vNormal;
varying vec4 vColor;
varying vec3 vWorldPos;

void main() {
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    vWorldPos = worldPos.xyz;
    vNormal = mat3(uModel) * aNormal;
    vColor = aColor;
    gl_Position = uMVP * vec4(aPosition, 1.0);
}
)";

const char* FRAGMENT_SHADER = R"(
precision mediump float;

varying vec3 vNormal;
varying vec4 vColor;
varying vec3 vWorldPos;

uniform vec3 uLightDir;
uniform vec3 uAmbientColor;
uniform vec3 uFogColor;
uniform float uFogNear;
uniform float uFogFar;

void main() {
    vec3 normal = normalize(vNormal);
    float diff = max(dot(normal, normalize(uLightDir)), 0.0);

    // Specular
    vec3 viewDir = normalize(-vWorldPos);
    vec3 halfDir = normalize(normalize(uLightDir) + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), 32.0);

    vec3 diffuse = vColor.rgb * diff;
    vec3 ambient = vColor.rgb * uAmbientColor;
    vec3 specular = vec3(0.3) * spec;

    vec3 color = ambient + diffuse + specular;

    // Distance fog
    float dist = length(vWorldPos);
    float fog = clamp((dist - uFogNear) / (uFogFar - uFogNear), 0.0, 1.0);
    color = mix(color, uFogColor, fog);

    gl_FragColor = vec4(color, vColor.a);
}
)";

// Simple sky shader
const char* SKY_VERTEX_SHADER = R"(
attribute vec3 aPosition;
varying vec3 vPos;
void main() {
    vPos = aPosition;
    gl_Position = vec4(aPosition, 1.0);
}
)";

const char* SKY_FRAGMENT_SHADER = R"(
precision mediump float;
varying vec3 vPos;
uniform vec3 uTopColor;
uniform vec3 uBottomColor;
void main() {
    float t = vPos.y * 0.5 + 0.5;
    vec3 color = mix(uBottomColor, uTopColor, t);
    gl_FragColor = vec4(color, 1.0);
}
)";

#endif // SHADERS_H
