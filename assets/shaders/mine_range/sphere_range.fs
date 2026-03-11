#version 300 es
precision highp float;

in vec3 vNormal;
in vec3 vWorldPos;

uniform vec3 viewPos;
uniform vec4 baseColor;
uniform float realTime;

out vec4 finalColor;

void main() {
    vec3 n = normalize(vNormal);
    vec3 v = normalize(viewPos - vWorldPos);

    float fresnel = 1.0 - abs(dot(n, v));
    fresnel = pow(fresnel, 3.0);
    float pulse = sin(realTime * 2.0) * 0.1 + 0.9;

    float grid = sin(vWorldPos.y * 5.0 - realTime * 3.0) * 0.5 + 0.5;
    grid = step(0.97, grid) * 0.2;

    vec3 color = baseColor.rgb * 1.5;
    float alpha = (fresnel + grid) * baseColor.a * pulse;

    finalColor = vec4(color, alpha);
}