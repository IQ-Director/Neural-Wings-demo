#version 300 es 
precision highp float;
in vec3 fragPosition;
in vec3 fragNormal;
in float v_linearDepth;

uniform vec3 viewPos;
uniform vec4 baseColor;

uniform float near;
uniform float far;

uniform float realTime;
uniform float gameTime;

out vec4 finalColor;

void main() {
    vec3 viewDir = normalize(viewPos - fragPosition);
    finalColor = vec4(viewDir * 0.5f + 0.5f, 1);
}
