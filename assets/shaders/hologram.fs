#version 300 es 
precision highp float;
in vec3 fragPosition;
in vec3 fragNormal;
uniform float realTime;
uniform vec4 baseColor;
out vec4 finalColor;

void main() {
    float scanline = sin(fragPosition.y * 10.0f - realTime * 10.0f);
    scanline = step(0.9f, scanline);

    vec3 normal = normalize(fragNormal);
    float edge = 1.0f - abs(normal.z);

    float flicker = sin(realTime * 50.0f) > 0.9f ? 0.5f : 1.0f;

    vec3 holoColor = vec3(0.2f, 1.0f, 0.4f);
    float finalAlpha = (edge * 0.5f + scanline * 0.5f) * flicker;

    finalColor = vec4(holoColor, finalAlpha);
}