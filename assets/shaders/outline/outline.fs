#version 300 es 
precision highp float;
uniform vec4 u_color;
uniform float realTime;

out vec4 finalColor;

void main() {
    float brightness = 0.8f + 0.5f * sin(realTime * 20.0f);
    vec3 b = clamp(u_color.rgb * brightness / 255.0f, 0.0f, 1.0f);
    finalColor = vec4(b, u_color.a / 255.0f);
}