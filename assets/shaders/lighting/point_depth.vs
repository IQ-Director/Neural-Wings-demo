#version 300 es 
precision highp float;
in vec3 vertexPosition;
uniform mat4 model;
uniform mat4 lightVP;

out vec3 vWorldPos;

void main() {
    vec4 worldPos = model * vec4(vertexPosition, 1.0f);
    vWorldPos = worldPos.xyz;
    gl_Position = lightVP * worldPos;
}