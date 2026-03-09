#version 300 es
precision highp float;

in vec3 vertexPosition;
in vec3 vertexNormal;

uniform mat4 u_mvp;
uniform mat4 transform;
uniform float u_radius;

out vec3 vNormal;
out vec3 vWorldPos;

void main() {
    vNormal = normalize(vec3(transform * vec4(vertexNormal, 0.0f)));

    vec3 scaledPos = vertexPosition * u_radius;

    vWorldPos = vec3(transform * vec4(scaledPos, 1.0f));

    gl_Position = u_mvp * vec4(scaledPos, 1.0f);
}