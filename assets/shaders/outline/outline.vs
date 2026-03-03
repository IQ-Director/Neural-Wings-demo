#version 300 es 
precision highp float;
in vec3 vertexPosition;
in vec3 vertexNormal;

uniform mat4 u_mvp;
uniform float u_outlineWidth;
uniform float realTime;
void main() {
    vec3 normal = normalize(vertexPosition);
    vec3 offsetPos = vertexPosition + normal * u_outlineWidth * (sin(realTime * 2.0f) + 1.0f);
    gl_Position = u_mvp * vec4(offsetPos, 1.0f);
}